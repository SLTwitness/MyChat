#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <iostream>
#include <sstream>
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server) :
	_socket(io_context), _server(server), b_close(false), b_head_parse(false)
{
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
	session_id = boost::uuids::to_string(a_uuid);
	recv_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
	std::cout << "CSession destruct" << std::endl;
}

void CSession::Start()
{
	AsyncReadHead(HEAD_TOTAL_LEN);
}

tcp::socket& CSession::GetSocket()
{
	return _socket;
}

std::string CSession::GetSessionId()
{
	return session_id;
}

void CSession::SetUserId(int uid)
{
	user_uid = uid;
}

int CSession::GetUserId()
{
	return user_uid;
}

void CSession::Send(std::string msg, short msgid)
{
	std::lock_guard<std::mutex> lock(send_lock);
	int send_que_size = send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << session_id << " send que fulled,size is " << MAX_SENDQUE << std::endl;
		return;
	}

	send_que.push(make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
	if (send_que_size > 0) {
		return;
	}
	auto& msg_node = send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::AsyncReadHead(int total_len)
{
	auto self = shared_from_this();
	asyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		try {
			if (ec) {
				cout << "handle read failed,error is " << ec.what() << endl;
				Close();
				_server->ClearSession(session_id);
				return;
			}

			if (bytes_transfered < HEAD_TOTAL_LEN) {
				cout << "read length not match,read [" << bytes_transfered << "],total [" 
					<< HEAD_TOTAL_LEN << "]" << endl;
				Close();
				_server->ClearSession(session_id);
				return;
			}

			recv_head_node->Clear();
			memcpy(recv_head_node->_data, _data, bytes_transfered);

			//获取头部MSG_ID
			short msg_id = 0;
			memcpy(&msg_id, recv_head_node->_data, HEAD_ID_LEN);
			//网络字节序转化为本地字节序
			msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
			std::cout << "msg_id is " << msg_id << std::endl;

			//id非法
			if (msg_id > MAX_LENGTH) {
				cout << "invalid msg_id is " << msg_id << endl;
				_server->ClearSession(session_id);
				return;
			}
			short msg_len = 0;
			memcpy(&msg_len, recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
			//网络字节序转化为本地字节序
			msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
			cout << "msg_len is " << msg_len << endl;

			//id非法
			if (msg_len > MAX_LENGTH) {
				std::cout << "invalid data length is " << msg_len << endl;
				_server->ClearSession(session_id);
				return;
			}

			recv_msg_node = make_shared<RecvNode>(msg_len, msg_id);
			AsyncReadBody(msg_len);
		}	
		catch (std::exception& e) {
			cout << "Exception code is " << e.what() << endl;
		}
		});
}

void CSession::AsyncReadBody(int length)
{
	auto self = shared_from_this();
	asyncReadFull(length, [self, this, length](const boost::system::error_code& ec, std::size_t bytesTransfered) {
		try {
			if (ec) {
				cout << "handle read failed,error is " << ec.what() << endl;
				Close();
				_server->ClearSession(session_id);
				return;
			}

			if (bytesTransfered < length) {
				cout << "read length not match,read [" << bytesTransfered << "],length [" << length << "]" << endl;
				Close();
				_server->ClearSession(session_id);
				return;
			}

			memcpy(recv_msg_node->_data, _data, bytesTransfered);
			recv_msg_node->cur_len += bytesTransfered;
			recv_msg_node->_data[recv_msg_node->total_len] = '\0';
			cout << "receive data is " << recv_msg_node->_data << endl;
			LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), recv_msg_node));
			AsyncReadHead(HEAD_TOTAL_LEN);
		}
		catch (std::exception& e) {
			cout << "Exception code is " << e.what() << endl;
		}
		});
}

void CSession::Close()
{
	_socket.close();
	b_close = true;
}

std::shared_ptr<CSession> CSession::SharedSelf()
{
	return shared_from_this();
}


void CSession::asyncReadFull(std::size_t maxLength, 
	std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	std::memset(_data, 0, MAX_LENGTH);
	asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, 
	std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	auto self = shared_from_this();
	_socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len),
		[read_len, total_len, handler, self](const boost::system::error_code& ec, std::size_t bytesTransfered) {
			if (ec) {
				handler(ec, read_len + bytesTransfered);
				return;
			}

			if (read_len + bytesTransfered >= total_len) {
				handler(ec, read_len + bytesTransfered);
				return;
			}

			self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
		});
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self)
{
	try {
		if (!error) {
			std::lock_guard<std::mutex> lock(send_lock);
			send_que.pop();
			if (!send_que.empty()) {
				auto& msg_node = send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else {
			std::cout << "handle write failed,error is " << error.what() << std::endl;
			Close();
			_server->ClearSession(session_id);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception code: " << e.what() << std::endl;
	}
}


LogicNode::LogicNode(shared_ptr<CSession> session, shared_ptr<RecvNode> recvNode) :
	_session(session), recv_node(recvNode)
{
}
