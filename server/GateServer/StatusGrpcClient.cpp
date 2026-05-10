#include "StatusGrpcClient.h"

StatusConPool::StatusConPool(std::string host, std::string port, size_t poolSize) :
	pool_size(poolSize), _host(host), _port(port), b_stop(false)
{
	for (size_t i = 0; i < poolSize; i++) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, 
			grpc::InsecureChannelCredentials());
		_con.push(StatusService::NewStub(channel));
	}
}

std::unique_ptr<StatusService::Stub> StatusConPool::getCon() {
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this] {
		if (b_stop) {
			return true;
		}
		return !_con.empty();
		});
	if (b_stop) {
		return nullptr;
	}
	auto context = std::move(_con.front());
	_con.pop();
	return context;
}

void StatusConPool::returnCon(std::unique_ptr<StatusService::Stub> context) {
	std::lock_guard<std::mutex> lock(_mtx);
	if (b_stop) {
		return;
	}
	_con.push(std::move(context));
	_cond.notify_one();
}

StatusConPool::~StatusConPool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	Close();
	while (!_con.empty()) {
		_con.pop();
	}
}

void StatusConPool::Close() {
	b_stop = true;
	_cond.notify_all();
}


GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	ClientContext context;
	GetChatServerRsp reply;
	GetChatServerReq request;
	request.set_uid(uid);
	auto stub = _pool->getCon();
	Status status = stub->GetChatServer(&context, request, &reply);
	Defer defer([&stub, this]() {
		_pool->returnCon(std::move(stub));
		});
	if (!status.ok()) {
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
	return reply;
}

StatusGrpcClient::StatusGrpcClient() 
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["StatusServer"]["Host"];
	std::string port = gCfgMgr["StatusServer"]["Port"];
	_pool.reset(new StatusConPool(host, port, 5));
}
