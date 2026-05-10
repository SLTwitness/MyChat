#include "MysqlMgr.h"

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& passwd)
{
    return _dao.RegistUser(name, email, passwd);
}

bool MysqlMgr::CheckPasswd(const std::string& email, const std::string& passwd, UserInfo& userInfo)
{
    return false;
}
