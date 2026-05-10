#include "CServer.h"
#include "AsioIOServicePool.h"
#include "CSession.h"
#include "UserMgr.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short port)
	:_ioc(ioc), _port(port), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
	std::cout << "Server start success,listen on port: " << _port << std::endl;
	StartAccept();
}

void CServer::ClearSession(std::string session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) {
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}

	{
		std::lock_guard<mutex> lock(_mtx);
		_sessions.erase(session_id);
	}
}

void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& err)
{
	std::cout << "new client connected" << std::endl;		//

	if (!err) {
		new_session->Start();
		lock_guard<mutex> lock(_mtx);
		_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed,err is " << err.what() << endl;
	}

	StartAccept();
}

void CServer::StartAccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}
