//
// Created by Autumn Sound on 2024/9/6.
//

#include "EngineWebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

EngineWebSocketSession::EngineWebSocketSession(websocket::stream<tcp::socket> ws)
        : ws_(std::move(ws)), timer_(ws_.get_executor()) {
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
        send_frame();
    }
    if (message == "clear") {
        engine_.clear();
    }
    if (message.starts_with("fps")) {
        auto pos = message.find('=');
        if (pos != std::string::npos) {
            fps_ = std::stoi(message.substr(pos + 1));
            if (fps_ == 0) {
                timer_.cancel();
            } else {
                timer_.expires_after(std::chrono::milliseconds(1000 / fps_));
                timer_.async_wait(
                        boost::beast::bind_front_handler(&EngineWebSocketSession::on_timer, shared_from_this()));
            }
        }
    }
//    if (message == "close") {
//        ws_.async_close(websocket::close_code::normal,
//                        boost::beast::bind_front_handler(&EngineWebSocketSession::on_write, shared_from_this()));
//    }
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

void EngineWebSocketSession::on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        fail(ec, "write");
        return;
    }
}

void EngineWebSocketSession::on_timer(boost::system::error_code ec) {
    if (ec && ec != boost::asio::error::operation_aborted) {
        fail(ec, "timer");
        return;
    }
    send_frame();
    timer_.expires_after(std::chrono::milliseconds(1000 / fps_));
    timer_.async_wait(boost::beast::bind_front_handler(&EngineWebSocketSession::on_timer, shared_from_this()));
}

void EngineWebSocketSession::send_frame() {
    // 发送帧
    frame_buffer_ = engine_.get_frame_buffer();
    ws_.binary(true);
    ws_.async_write(boost::asio::buffer(frame_buffer_),
                    boost::beast::bind_front_handler(&EngineWebSocketSession::on_write, shared_from_this()));
}
