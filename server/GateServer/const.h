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