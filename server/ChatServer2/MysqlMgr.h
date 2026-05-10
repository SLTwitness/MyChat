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
	
	bool AddFriendApply(const int& from, const int& to);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit = 10);

	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);

	bool AuthFriendApply(const int& from, const int& to);
	bool AddFriend(const int& from, const int& to, std::string back_name);

	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_info);
private:
	MysqlMgr() = default;
	MysqlDao _dao;
};

