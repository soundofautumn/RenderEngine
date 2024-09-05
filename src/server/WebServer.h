//
// Created by 邱姜铭 on 2024/9/5.
//

#ifndef RENDERENGINE_WEBSERVER_H
#define RENDERENGINE_WEBSERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebServer {
public:
    explicit WebServer(int port);

    void start();

private:
    int port;
    net::io_context ioc;
    std::shared_ptr<tcp::acceptor> acceptor;

    void initHttp();
    void initWebSocket();
};

#endif //RENDERENGINE_WEBSERVER_H
