//
// Created by Autumn Sound on 2024/9/6.
//

#include "EngineWebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

EngineWebSocketSession::EngineWebSocketSession(websocket::stream<tcp::socket> ws)
        : ws_(std::move(ws)) {
}

void EngineWebSocketSession::run() {
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

void EngineWebSocketSession::on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        fail(ec, "read");
        return;
    }
    logger::info("EngineWebSocketSession::on_read");
    ws_.async_write(boost::asio::buffer("output.bmp"),
                    boost::beast::bind_front_handler(&EngineWebSocketSession::on_write, shared_from_this()));
}

void EngineWebSocketSession::on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        fail(ec, "write");
        return;
    }
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

