#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket) :_socket(std::move(socket)) {};

void HttpConnection::Start() {
	auto self = shared_from_this();				//获取当前HttpConnection对象的shared_ptr
	self->CheckDeadline();					//开始时就启动超时保护，以防客户端建立连接但不发消息而产生冗余HttpConnection
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, std::size_t bytes_transferred) {		//从socket读取buffer request
		try {
			if (ec) {
				std::cout << "http read err is " << ec.what() << std::endl;
				return;
			}

			boost::ignore_unused(bytes_transferred);		//忽视此参数，避免未使用变量警告
			self->HandleReq();
		}
		catch(std::exception& exp){
			std::cout << "exception is " << exp.what() << std::endl;
		}
	});
}

unsigned char ToHex(unsigned char x) {
	return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x) {
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

std::string UrlEncode(const std::string& str) {
	std::string strTemp = "";
	for (size_t i = 0; i < str.length(); i++) {
		if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~')) {
			strTemp += str[i];
		}
		else if (str[i] == ' ') {
			strTemp += "+";
		}
		else {														//处理特殊字符
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}

std::string UrlDecode(const std::string& str) {
	std::string strTemp = "";
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%') {
			assert(i + 2 < str.length());
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

void HttpConnection::HandleReq() {
	_response.version(_request.version());
	_response.keep_alive(false);

	if (_request.method() == http::verb::get) {
		PreParseGetParam();
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());	//target()返回URL路径
		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";		//写响应
			WriteResponse();
			return;
		}

		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");		//set(field,value)设置HTTP属性
		WriteResponse();
		return;
	}

	if (_request.method() == http::verb::post) {
		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}

		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}

void HttpConnection::PreParseGetParam()
{
	auto url = _request.target();			//提取url：/get_test?key1=value1&key2=value2（http://localhost:8080/get_test?key1=value1&key2=value2）
	auto query_pos = url.find('?');
	if (query_pos == std::string::npos) {		//没找到
		_get_url = url;
		return;
	}

	_get_url = url.substr(0, query_pos);
	std::string query_string = url.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while (true) {
		pos = query_string.find('&');
		if (pos == std::string::npos) break;
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos));
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}

void HttpConnection::WriteResponse() {
	auto self = shared_from_this();
	_response.content_length(_response.body().size());		//设置响应长度
	http::async_write(_socket, _response, [self](beast::error_code ec, std::size_t bytes_transferred) {		//向socket写数据
		self->_socket.shutdown(tcp::socket::shutdown_send,ec);		//关闭发送通道
		self->deadline_.cancel();		//关闭超时检测
	});
}

void  HttpConnection::CheckDeadline() {
	auto self = shared_from_this();
	deadline_.async_wait([self](beast::error_code ec) {
		if (!ec) {
			self->_socket.close(ec);
		}
	});
}