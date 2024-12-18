//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_ENGINEWEBSOCKETSESSION_H
#define RENDERENGINE_ENGINEWEBSOCKETSESSION_H

#include "EngineManager.h"
#include "Server.h"
#include "engine.hpp"

using RenderCore::RenderEngine;
namespace websocket = boost::beast::websocket;
namespace http = boost::beast::http;

class EngineWebSocketSession : public std::enable_shared_from_this<EngineWebSocketSession> {
    std::string engine_name_;

    websocket::stream<tcp::socket> ws_;
    boost::beast::flat_buffer buffer_;
    boost::asio::steady_timer timer_;
    int fps_{0};
    // frame buffer
    RenderEngine::Buffer frame_buffer_;
    bool write_in_progress_{false};

   public:
    explicit EngineWebSocketSession(tcp::socket socket);

    void run(const http::request<http::string_body>& req);

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_timer(boost::system::error_code ec);

    void send_frame();

    void set_fps(int fps);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);
};

#endif  //RENDERENGINE_ENGINEWEBSOCKETSESSION_H
