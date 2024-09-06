//
// Created by Autumn Sound on 2024/9/6.
//

#include "EngineWebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

EngineWebSocketSession::EngineWebSocketSession(websocket::stream<tcp::socket> ws)
        : ws_(std::move(ws)) {
}

void EngineWebSocketSession::run() {
    engine_.init(800, 600);
    engine_.clear();
    engine_.draw_line({0, 0}, {799, 599}, {.color = Colors::Red}, LineAlgorithm::DDA);
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

void EngineWebSocketSession::on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        fail(ec, "read");
        return;
    }
    const std::string message = boost::beast::buffers_to_string(buffer_.data());
    buffer_.consume(buffer_.size());
    logger::info("Received message: {}", message);

    if (message == "get") {
        frame_buffer_ = engine_.get_frame_buffer();
        ws_.binary(true);
        ws_.async_write(boost::asio::buffer(frame_buffer_),
                        boost::beast::bind_front_handler(&EngineWebSocketSession::on_write, shared_from_this()));
        return;
    }
    ws_.binary(false);
    ws_.async_write(boost::asio::buffer("Unknown command"),
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

