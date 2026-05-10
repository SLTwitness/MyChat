#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPCPool {
public:
	RPCPool(size_t poolsize, std::string host, std::string port);
	~RPCPool();

	std::unique_ptr<VerifyService::Stub> getCon();			//拿（借）链接
	void returnCon(std::unique_ptr<VerifyService::Stub> context);		//还链接
	void Close();
private:
	std::atomic<bool> b_stop;		//连接池是否已关闭
	size_t pool_Size;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<VerifyService::Stub>> _cons;
	std::condition_variable _cond; 
	std::mutex _mtx;
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVerifyRsp GetVerifyCode(std::string email);

private:
	VerifyGrpcClient();
	std::unique_ptr<RPCPool> _pool;
};

