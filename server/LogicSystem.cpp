#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"

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
        root["code"] = rsp.code();                  //测试用：返回验证码
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });
}
