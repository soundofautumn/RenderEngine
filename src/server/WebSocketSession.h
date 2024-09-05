//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_WEBSOCKETSESSION_H
#define RENDERENGINE_WEBSOCKETSESSION_H

#include "Server.h"

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    websocket::stream<tcp::socket> ws_;

public:
    explicit WebSocketSession(tcp::socket socket);

    void do_accept(const http::request<boost::beast::http::string_body>& req);

    void on_accept(boost::system::error_code ec);

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

};


#endif //RENDERENGINE_WEBSOCKETSESSION_H
