#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"
#include "CServer.h"
#include "LogicSystem.h"
using namespace std;

class CServer;
class LogicSystem;

class CSession:public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	void Start();
	tcp::socket& GetSocket();
	std::string GetSessionId();
	void SetUserId(int uid);
	int GetUserId();
	void Send(std::string msg, short msgid);
	void AsyncReadHead(int total_len);
	void AsyncReadBody(int length);
	void Close();
	std::shared_ptr<CSession> SharedSelf();

private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler);

	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	
	tcp::socket _socket;
	std::string session_id;
	int user_uid;
	char _data[MAX_LENGTH];
	CServer* _server;
	std::mutex send_lock;
	bool b_close;
	bool b_head_parse;
	std::queue<shared_ptr<SendNode>> send_que;
	std::shared_ptr<RecvNode> recv_msg_node;
	std::shared_ptr<MsgNode> recv_head_node;
};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);
private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> recv_node;
};

