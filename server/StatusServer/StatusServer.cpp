#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "const.h"
#include "ConfigMgr.h"
//#include "hiredis.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "AsioIOServicePool.h"
#include <memory>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <grpcpp/grpcpp.h>
#include "StatusServiceImp.h"

void RunServer() {
    auto& cfg = ConfigMgr::Inst();

    std::string server_address(cfg["StatusServer"]["Host"] + ":" + cfg["StatusServer"]["Port"]);
    StatusServiceImp service;
    
    grpc::ServerBuilder builder;
    //监听端口
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    //注册服务
    builder.RegisterService(&service);

    //构建并启动gRPC服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    //创建事件轮询
    boost::asio::io_context io_context;
    //注册系统信号监听  SIGINT(Ctrl+C)  SIGTERM(kill)
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

    //异步等待信号，触发回调并退出
    signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server ..." << std::endl;
            server->Shutdown();
        }
        });

    //放在单独的线程中跑
    std::thread t([&io_context]() { io_context.run(); });

    //等待服务器关闭
    server->Wait();
    io_context.stop();
    t.join();
}

int main()
{
    try {
        RunServer();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
