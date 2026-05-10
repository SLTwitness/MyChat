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
			//std::cout << "execute timer alive query,cur is " << secTime << std::endl;
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