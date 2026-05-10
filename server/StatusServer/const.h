#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <map>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "hiredis.h"
#include <cassert>

namespace beast = boost::beast;		//命名空间别名
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;	//类型别名

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,

	VerifyExpired = 1003,
	VerifyCodeErr = 1004,
	UserExist = 1005,

	PasswdInvalid = 1006,
	UidInvalid = 1007,
	TokenInvalid = 1008,
};

class Defer {
public:
	Defer(std::function<void()> func) :_func(func) {}
	~Defer() {
		_func();
	}
private:
	std::function<void()> _func;
};

#define MAX_LENGTH 1024*2
#define HEAD_TOTAL_LEN 4		//头部总长度
#define HEAD_ID_LEN 2		//头部id长度
#define HEAD_DATA_LEN 2		//头部数据长度
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005,		//用户登录
	MSG_CHAT_LOGIN_RSP = 1006,		//用户登录回包
};

#define USERIPPREFIX "uip_"
#define USERTOKENPREFIX "utoken_"
#define IPCOUNTPREFIX "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT "logincount"