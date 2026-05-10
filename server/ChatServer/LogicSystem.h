#pragma once

#include "const.h"
#include <functional>
#include "data.h"

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

class CSession;
class LogicNode;

using FunCallBack = std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)>;
class LogicSystem:public Singleton<LogicSystem> {
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	bool HandleGet(std::string path,std::shared_ptr<HttpConnection> con);
	bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
	void RegGet(std::string url,HttpHandler handler);
	void RegPost(std::string url, HttpHandler handler);

	void PostMsgToQue(std::shared_ptr<LogicNode> msg);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;

	void DealMsg();
	void RegisterCallBack();
	void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

	bool isPureDigit(const std::string& str);
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	void GetUserByName(std::string name, Json::Value& rtvalue);
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);

	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);

	std::queue<std::shared_ptr<LogicNode>> msg_que;
	std::condition_variable _consume;
	std::thread worker_thread;
	std::mutex _mtx;
	bool b_stop;
	std::map<short, FunCallBack> fun_callbacks;
};