#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServer.h"
#include "ConfigMgr.h"

int main()
{
    ConfigMgr gCfgMgr;
    std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());

    try {
        unsigned short port = static_cast<unsigned short>(8080);
        net::io_context ioc{ 1 };                                   //提示最多几个线程调用
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            
            ioc.stop();
        });
        std::make_shared<CServer>(ioc,port)->Start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        ioc.run();                      //开启事件轮询
    }
    catch (std::exception const& e) {                       //以防抛异常崩溃
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}