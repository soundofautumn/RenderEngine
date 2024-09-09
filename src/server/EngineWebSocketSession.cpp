//
// Created by Autumn Sound on 2024/9/6.
//

#include "EngineWebSocketSession.h"
#include "handle_request.h"

extern void fail(boost::system::error_code ec, char const *what);

EngineWebSocketSession::EngineWebSocketSession(websocket::stream<tcp::socket> ws)
        : ws_(std::move(ws)), timer_(ws_.get_executor()) {
}

void EngineWebSocketSession::run() {
    ws_.binary(true);
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

void EngineWebSocketSession::set_fps(int fps) {
    fps_ = fps;
    timer_.cancel();
    if (fps_ != 0) {
        timer_.expires_after(std::chrono::milliseconds(1000 / fps_));
        timer_.async_wait(boost::beast::bind_front_handler(&EngineWebSocketSession::on_timer, shared_from_this()));
    }
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
    if (message.starts_with("set_fps")) {
        auto pos = message.find(' ');
        if (pos != std::string::npos) {
            set_fps(std::stoi(message.substr(pos + 1)));
        }
    }
    handle_engine_request(message, engine_);
    ws_.async_read(buffer_,
                   boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, shared_from_this()));
}

void EngineWebSocketSession::on_timer(boost::system::error_code ec) {
    if (ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }
        fail(ec, "timer");
        return;
    }
    send_frame();
    if (fps_ != 0) {
        timer_.expires_after(std::chrono::milliseconds(1000 / fps_));
        timer_.async_wait(boost::beast::bind_front_handler(&EngineWebSocketSession::on_timer, shared_from_this()));
    }
}

void EngineWebSocketSession::send_frame() {
    std::lock_guard lock(frame_buffer_mutex_);
    // 发送帧
    if (write_in_progress_) {
        logger::warn("Write in progress, frame dropped");
        return;
    }
    frame_buffer_ = engine_.get_frame_buffer();
    write_in_progress_ = true;
    ws_.async_write(
            boost::asio::buffer(frame_buffer_),
            boost::beast::bind_front_handler(&EngineWebSocketSession::on_write, shared_from_this()));
}

void EngineWebSocketSession::on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        fail(ec, "write");
        write_in_progress_ = false;
        timer_.cancel();
        return;
    }
    write_in_progress_ = false;
}


