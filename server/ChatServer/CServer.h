#pragma once

#include "const.h"
#include <thread>

using namespace std;

class CSession;

class CServer:public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, unsigned short port);
	void ClearSession(std::string);
private:
	void HandleAccept(shared_ptr<CSession>, const boost::system::error_code& err);
	void StartAccept();
	tcp::acceptor _acceptor;		//接收对端链接，也需要ioc作为媒介通信
	net::io_context& _ioc;			//上下文服务，socket通信的关键（引用保证唯一性和生命周期）
	unsigned short _port;
	std::map<std::string, shared_ptr<CSession>> _sessions;
	std::mutex _mtx;
};