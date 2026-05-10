#include "MysqlMgr.h"

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& passwd)
{
    return _dao.RegistUser(name, email, passwd);
}

bool MysqlMgr::CheckPasswd(const std::string& email, const std::string& passwd, UserInfo& userInfo)
{
    return _dao.CheckPasswd(email, passwd, userInfo);
}

bool MysqlMgr::AddFriendApply(const int& from, const int& to)
{
    return _dao.AddFriendApply(from,to);
}

bool MysqlMgr::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit)
{
    return _dao.GetApplyList(touid, applyList, begin, limit);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
    return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
    return _dao.GetUser(name);
}

bool MysqlMgr::AuthFriendApply(const int& from, const int& to)
{
    return _dao.AuthFriendApply(from, to);
}

bool MysqlMgr::AddFriend(const int& from, const int& to, std::string back_name)
{
    return _dao.AddFriend(from, to, back_name);
}

bool MysqlMgr::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_info) {
    return _dao.GetFriendList(self_id, user_info);
}