#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::StatusService;

class StatusConPool {
public:
	StatusConPool(std::string host, std::string port, size_t poolSize);
	std::unique_ptr<StatusService::Stub> getCon();
	void returnCon(std::unique_ptr<StatusService::Stub> context);
	void Close();
	~StatusConPool();
private:
	std::string _host;
	std::string _port;
	size_t pool_size;
	std::atomic<bool> b_stop;
	std::queue<std::unique_ptr<StatusService::Stub>> _con;
	std::mutex _mtx;
	std::condition_variable _cond;
};

class StatusGrpcClient :public Singleton<StatusGrpcClient> {
	friend class Singleton<StatusGrpcClient>;
public:
	GetChatServerRsp GetChatServer(int uid);
	LoginRsp Login(int uid, std::string token);
	~StatusGrpcClient() = default;
private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> _pool;
};