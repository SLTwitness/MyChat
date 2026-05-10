#include "MysqlDao.h"
#include "ConfigMgr.h"

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, int poolSize) :
	_url(url), _user(user), _passwd(passwd), _schema(schema), pool_size(poolSize), b_stop(false)
{
	try {
		for (int i = 0; i < poolSize; i++) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();		//获取mysql驱动
			auto* con = driver->connect(url, user, passwd);
			con->setSchema(schema);
			auto curTime = std::chrono::system_clock::now().time_since_epoch();
			long long secTime = std::chrono::duration_cast<std::chrono::seconds>(curTime).count();
			_pool.push(std::make_unique<SqlCon>(con, secTime));			//把con给unique_prt管理，自动释放
		}

		check_thread = std::thread([this]() {
			while (!b_stop) {
				CheckCon();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlPool init error: " << e.what()
			<< " code=" << e.getErrorCode()
			<< " SQLState=" << e.getSQLState() << std::endl;
	}
}

MysqlPool::~MysqlPool()
{
	Close();
	if (check_thread.joinable()) {
		check_thread.join();
	}
}

void MysqlPool::CheckCon()
{
	std::lock_guard<std::mutex> guard(_mtx);
	int poolSize = _pool.size();
	auto curTime = std::chrono::system_clock::now().time_since_epoch();
	long long secTime = std::chrono::duration_cast<std::chrono::seconds>(curTime).count();
	for (int i = 0; i < poolSize; i++) {
		auto con = std::move(_pool.front());
		_pool.pop();
		Defer defer([this, &con]() {
			_pool.push(std::move(con));
			});

		if (secTime - con->last_time < 60) {
			continue;
		}
		try {
			std::unique_ptr<sql::Statement> state(con->_con->createStatement());
			state->executeQuery("SELECT 1");
			con->last_time = secTime;
			std::cout << "execute timer alive query,cur is " << secTime << std::endl;
		}
		catch(sql::SQLException& e){
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			
			//出错则重新创建链接，并替换原链接
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* new_con = driver->connect(_url, _user, _passwd);
			new_con->setSchema(_schema);
			con->_con.reset(new_con);
			con->last_time = secTime;
		}
	}
}

std::unique_ptr<SqlCon> MysqlPool::getCon()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this]() {
		if (b_stop) {
			return true;
		}
		return !_pool.empty();
		});
	if (b_stop) {
		return nullptr;
	}
	std::unique_ptr<SqlCon> con(std::move(_pool.front()));
	_pool.pop();
	return con;
}

void MysqlPool::returnCon(std::unique_ptr<SqlCon> con)
{
	std::unique_lock<std::mutex> lock(_mtx);
	if (b_stop) {
		return;
	}
	_pool.push(std::move(con));
	_cond.notify_one();
}

void MysqlPool::Close()
{
	std::lock_guard<std::mutex> lock(_mtx);
	b_stop = true;
	_cond.notify_all();
}



MysqlDao::MysqlDao()
{
	auto& cfg = ConfigMgr::Inst();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& user = cfg["Mysql"]["User"];
	const auto& passwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	_pool.reset(new MysqlPool(host + ":" + port, user, passwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	_pool->Close();
}

int MysqlDao::RegistUser(const std::string& name, const std::string& email, const std::string& passwd)
{
	std::cout << "execute regist" << std::endl;
	auto con = _pool->getCon();
	try {
		if (con == nullptr) {
			return 0;
		}
		std::cout << "get con" << std::endl;
		std::unique_ptr<sql::PreparedStatement> state(con->_con->prepareStatement("CALL regist_user(?,?,?,@result)"));
		state->setString(1, name);
		state->setString(2, email);
		state->setString(3, passwd);

		state->execute();
		
		std::cout << "executed" << std::endl;

		std::unique_ptr<sql::Statement> stateRes(con->_con->createStatement());
		std::unique_ptr<sql::ResultSet> res(stateRes->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result:" << result << std::endl;
			_pool->returnCon(std::move(con));
			return result;
		}
		_pool->returnCon(std::move(con));
		return -1;
	}
	catch (sql::SQLException& e) {
		_pool->returnCon(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return -1;
	}
}

bool MysqlDao::CheckPasswd(const std::string& email, const std::string& passwd, UserInfo& userInfo)
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer derfer([this, &con]() {
		_pool->returnCon(std::move(std::move(con)));
		});

	try {
		if (con == nullptr) {
			return false;
		}

		//准备SQL查询语句
		std::unique_ptr<sql::PreparedStatement> sqlstm(con->_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
		sqlstm->setString(1, email);

		//执行查询语句
		std::unique_ptr<sql::ResultSet> res(sqlstm->executeQuery());
		std::string true_passwd = "";
		if (res->next()) {
			true_passwd = res->getString("passwd");
			std::cout << "Password:" << true_passwd << std::endl;
		}
		else {
			std::cout << "user " << email << " not found" << std::endl;
			return false;
		}

		if (passwd != true_passwd) {
			return false;
		}

		userInfo.uid = res->getInt("uid");
		userInfo.name = res->getString("name");
		userInfo.email = email;
		userInfo.passwd = true_passwd;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "( MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
	return false;
}

bool MysqlDao::AddFriendApply(const int& from, const int& to) 
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("INSERT INTO friend_apply (from_uid,to_uid) value (?,?) "
		"ON DUPLICATE KEY UPDATE from_uid=from_uid,to_uid=to_uid "));
		pstmt->setInt(1, from);
		pstmt->setInt(2, to);

		//执行语句，返回影响行数
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			return false;
		}

		return true;
	}
	catch (sql::SQLException& e){
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return false;
	}
}

bool MysqlDao::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit)
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("select apply.from_uid,apply.status,user.name, "
			"user.nick,user.sex,user.icon from friend_apply as apply join user on apply.from_uid=user.uid where apply.to_uid=? "
			"and apply.id > ? order by apply.id ASC LIMIT ?"));

		pstmt->setInt(1, touid);
		pstmt->setInt(2, begin);
		pstmt->setInt(3, limit);

		//执行语句，返回影响行数
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		while (res->next()) {
			auto uid = res->getInt("from_uid");
			auto name = res->getString("name");
			auto nick = res->getString("nick");
			auto sex = res->getInt("sex");
			auto icon = res->getString("icon");
			auto status = res->getInt("status");
			auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", icon, nick, sex, status);
			applyList.push_back(apply_ptr);
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return false;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid) 
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;

		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->passwd = res->getString("passwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");
			user_ptr->uid = uid;
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;

		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->passwd = res->getString("passwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");
			user_ptr->uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return nullptr;
	}
}

bool MysqlDao::AuthFriendApply(const int& from, const int& to)
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("UPDATE friend_apply SET status=1 "
			"WHERE from_uid = ? AND to_uid = ? "));
		//因为是向自己申请的，所以反过来
		pstmt->setInt(1, to);
		pstmt->setInt(2, from);

		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return false;
	}

	return true;
}

bool MysqlDao::AddFriend(const int& from, const int& to, std::string back_name)
{
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		con->_con->setAutoCommit(false);

		//插入认证方的好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("INSERT IGNORE INTO friend(self_id,friend_id,back) "
			"VALUES(?, ?, ?) "));
		pstmt->setInt(1, from);
		pstmt->setInt(2, to);
		pstmt->setString(3, back_name);

		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			con->_con->rollback();
			return false;
		}

		//插入申请方的好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt2(con->_con->prepareStatement("INSERT IGNORE INTO friend(self_id,friend_id,back) "
			"VALUES(?, ?, ?) "));
		pstmt2->setInt(1, to);
		pstmt2->setInt(2, from);
		pstmt2->setString(3, "");

		int rowAffected2 = pstmt2->executeUpdate();
		if (rowAffected2 < 0) {
			con->_con->rollback();
			return false;
		}

		//提交事务
		con->_con->commit();
		std::cout << "addfriend insert friends success" << std::endl;

		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return false;
	}

	return true;
}

bool MysqlDao::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_info_list) {
	auto con = _pool->getCon();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		_pool->returnCon(std::move(con));
		});

	try {
		//根据起始id和条数返回列表
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("select * from friend where self_id = ? "));
		pstmt->setInt(1, self_id);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		while (res->next()) {
			auto friend_id = res->getInt("friend_id");
			auto back = res->getString("back");
			//查询id对应信息
			auto user_info = GetUser(friend_id);
			if (user_info == nullptr) {
				continue;
			}

			user_info->back = user_info->name;
			user_info_list.push_back(user_info);
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << "(MySQL error code: " << e.getErrorCode();
		std::cerr << ",SQLState: " << e.getSQLState() << ")" << std::endl;
		return false;
	}

	return true;
}