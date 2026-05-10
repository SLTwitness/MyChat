#pragma once
#include "const.h"
#include <thread>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

class SqlCon {
public:
	SqlCon(sql::Connection* con, int64_t lasttime) :_con(con), last_time(lasttime) {}
	std::unique_ptr<sql::Connection> _con;
	int64_t last_time;
};

struct UserInfo {
	int uid;
	std::string name;
	std::string passwd;
	std::string email;
};

class MysqlPool {
public:
	MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, int poolSize);
	~MysqlPool();
	void CheckCon();
	std::unique_ptr<SqlCon> getCon();
	void returnCon(std::unique_ptr<SqlCon> con);
	void Close();
private:
	std::string _url;
	std::string _user;
	std::string _passwd;
	std::string _schema;
	int pool_size;
	std::queue<std::unique_ptr<SqlCon>> _pool;
	std::mutex _mtx;
	std::condition_variable _cond;
	std::atomic<bool> b_stop;
	std::thread check_thread;
};

class MysqlDao {
public:
	MysqlDao();
	~MysqlDao();
	int RegistUser(const std::string& name, const std::string& email, const std::string& passwd);
	/*bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& passwd);*/
	bool CheckPasswd(const std::string& name, const std::string& passwd, UserInfo& userInfo);
private:
	std::unique_ptr<MysqlPool> _pool;
};
