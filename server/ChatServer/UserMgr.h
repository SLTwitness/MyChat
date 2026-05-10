#pragma once
#include "Singleton.h"
#include <memory>
#include <mutex>
#include <map>

class CSession;

class UserMgr :public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;
public:
	~UserMgr();
	std::shared_ptr<CSession> GetSession(int uid);
	void SetUserSession(int uid, std::shared_ptr<CSession> session);
	void RmvUserSession(int uid);
private:
	UserMgr() = default;
	std::mutex session_mtx;
	std::unordered_map<int, std::shared_ptr<CSession>> uid_to_session;
};

