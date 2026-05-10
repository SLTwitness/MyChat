#pragma once
#include "const.h"
#include "MysqlDao.h"

class MysqlMgr:public Singleton<MysqlMgr>
{
	friend Singleton<MysqlMgr>;
public:
	~MysqlMgr() = default;
	int RegUser(const std::string& name, const std::string& email, const std::string& passwd);
	bool CheckPasswd(const std::string& email, const std::string& passwd, UserInfo& userInfo);
private:
	MysqlMgr() = default;
	MysqlDao _dao;
};

