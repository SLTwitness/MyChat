#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

std::unique_ptr<VerifyService::Stub> RPCPool::getCon()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this]() {
		if (b_stop) {
			return true;
		}
		return !_cons.empty();
		});

	if (b_stop) {
		return nullptr;		//停止提供资源
	}

	auto context = std::move(_cons.front());
	_cons.pop();
	return context;
}

void RPCPool::returnCon(std::unique_ptr<VerifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (b_stop) {
		return;
	}

	_cons.push(std::move(context));
	_cond.notify_one();				//唤起一个等待的线程
}

void RPCPool::Close()
{
	b_stop = true;
	_cond.notify_all();
}



GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email)
{
	ClientContext context;
	GetVerifyRsp reply;
	GetVerifyReq request;
	request.set_email(email);
	auto stub = _pool->getCon();
	Status status = stub->GetVerifyCode(&context, request, &reply);
	if (status.ok()) {
		_pool->returnCon(std::move(stub));
		return reply;
	}
	else {
		_pool->returnCon(std::move(stub));
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient() {
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VerifyServer"]["Host"];
	std::string port = gCfgMgr["VerifyServer"]["Port"];
	_pool.reset(new RPCPool(5, host, port));
}

RPCPool::RPCPool(size_t poolsize, std::string host, std::string port):
	pool_Size(poolsize), _host(host), _port(port), b_stop(false) {
	for (size_t i = 0; i < pool_Size; i++) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
			grpc::InsecureChannelCredentials());
		_cons.push(VerifyService::NewStub(channel));		//右值（临时对象）自动调用移动构造
	}
}

RPCPool::~RPCPool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	Close();
	while (!_cons.empty()) {
		_cons.pop();
	}
}