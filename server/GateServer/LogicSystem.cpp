#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "StatusGrpcClient.h"

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con)
{
    if (_get_handlers.find(path) == _get_handlers.end()) {      //确认map里是否有这个key对应的可调用对象
        return false;
    }
    
    _get_handlers[path](con);           //取出可调用对象handler(lambda)并传入参数con
    return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
    if (_post_handlers.find(path) == _post_handlers.end()) { 
        return false;
    }

    _post_handlers[path](con);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handlers.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    _post_handlers.insert(make_pair(url, handler));
}

LogicSystem::LogicSystem()                          //构造函数
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {            //键:"/get_test"    值:可调用对象
        beast::ostream(connection->_response.body()) << "receive get_test req" << std::endl;
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->_response.body()) << "param " << i << " key is " << elem.first << std::endl;
            beast::ostream(connection->_response.body()) << "param " << i << " value is " << elem.second<<std::endl;
        }
        });

    RegPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);

        if (!parse_success || !src_root.isMember("email")) {
            std::cout << "Fail to parse Json data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        auto email = src_root["email"].asString();
        GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });

    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);

        if (!parse_success || !src_root.isMember("email")) {
            std::cout << "Fail to parse Json data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        
        std::string verify_code;
        bool verify_success = RedisMgr::GetInstance()->Get("code_"+src_root["email"].asString(), verify_code);
        
        //redis中验证码是否过期
        if (!verify_success) {
            std::cout << "get verify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string json_str = root.toStyledString();
            beast::ostream(connection->_response.body()) << json_str;
            return true;
        }
        
        //redis中验证码是否错误
        if (verify_code != src_root["verifycode"].asString()) {
            std::cout << "verify code error " << std::endl;
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string json_str = root.toStyledString();
            beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //mysql数据库中用户是否存在
        int uid = MysqlMgr::GetInstance()->RegUser(src_root["user"].asString(), src_root["email"].asString(), src_root["passwd"].asString());
        if (uid == 0 || uid == -1) {
            std::cout << "user or email exist" << std::endl;
            root["error"] = ErrorCodes::UserExist;
            std::string json_str = root.toStyledString();
            beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        root["error"] = 0;
        root["email"] = src_root["email"];
        root["user"] = src_root["user"].asString();
        root["passwd"] = src_root["passwd"].asString();
        root["verifycode"] = src_root["verifycode"].asString();
        std::string json_str = root.toStyledString();
        beast::ostream(connection->_response.body()) << json_str;
        return true;
        });

    //用户登录验证
    RegPost("/user_login", [](std::shared_ptr<HttpConnection> con) {
        auto body_str = boost::beast::buffers_to_string(con->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        con->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        //解析JSON
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse Json data" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(con->_response.body()) << jsonstr;
            return true;
        }

        auto email = src_root["email"].asString();
        auto passwd = src_root["passwd"].asString();
        UserInfo userInfo;

        //检查邮箱密码
        bool passwd_valid = MysqlMgr::GetInstance()->CheckPasswd(email,passwd,userInfo);
        if (!passwd_valid) {
            std::cout << "user passwd not correct" << std::endl;
            root["error"] = ErrorCodes::PasswdInvalid;
            std::string jsonstr = root.toStyledString();
            beast::ostream(con->_response.body()) << jsonstr;
            return true;
        }

        //查询StatusServer找到合适连接
        std::cout << "before grpc call" << std::endl;
        auto reply = StatusGrpcClient::GetInstance()->GetChatServer(userInfo.uid);
        std::cout << "after grpc call" << std::endl;
        if (reply.error()) {
            std::cout << "grpc get chat server failed,error is " << reply.error() << std::endl;
            root["error"] = ErrorCodes::RPCFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(con->_response.body()) << jsonstr;
            return true;
        }

        std::cout << "succeed to load userinfo uid is " << userInfo.uid << std::endl;
        root["error"] = 0;
        root["email"] = email;
        root["uid"] = userInfo.uid;
        root["token"] = reply.token();
        root["host"] = reply.host();
        root["port"] = reply.port();
        std::string jsonstr = root.toStyledString();
        beast::ostream(con->_response.body()) << jsonstr;
        return true;
        });
}
