//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_ENGINEWEBSOCKETSESSION_H
#define RENDERENGINE_ENGINEWEBSOCKETSESSION_H

#include "Server.h"
#include "engine.h"

class EngineWebSocketSession :
        public std::enable_shared_from_this<EngineWebSocketSession> {
    websocket::stream<tcp::socket> ws_;
    boost::beast::flat_buffer buffer_;
    boost::asio::steady_timer timer_;
    int fps_{0};
    // frame buffer
    RenderEngine::Buffer frame_buffer_;
    RenderEngine engine_{};

public:
    explicit EngineWebSocketSession(websocket::stream<tcp::socket> ws);

    void run();

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_timer(boost::system::error_code ec);

    void send_frame();
};


#endif //RENDERENGINE_ENGINEWEBSOCKETSESSION_H
