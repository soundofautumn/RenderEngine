//
// Created by Autumn Sound on 2024/9/6.
//

#include "EngineWebSocketSession.h"

#include "handle_request.h"

extern void fail(boost::system::error_code ec, char const *what);

EngineWebSocketSession::EngineWebSocketSession(tcp::socket socket)
    : ws_(std::move(socket)), timer_(ws_.get_executor()) {}

void EngineWebSocketSession::run(const http::request<http::string_body> &req) {
    // find the engine name
    auto pos = req.target().find_last_of('/');
    if (pos == std::string::npos) {
        logger::error("Invalid request target: {}", req.target());
        return;
    }
    engine_name_ = req.target().substr(pos + 1);
    logger::debug("Engine name: {}", engine_name_);
    if (!EngineManager::get_instance().check_engine(engine_name_)) {
        logger::error("Engine not found: {}", engine_name_);
        return;
    }
    ws_.set_option(websocket::permessage_deflate());
    ws_.async_accept(req, [self = shared_from_this()](boost::system::error_code) {
        self->ws_.binary(true);
        self->ws_.async_read(self->buffer_,
            boost::beast::bind_front_handler(&EngineWebSocketSession::on_read, self));
    });
}

void EngineWebSocketSession::set_fps(int fps) {
    fps_ = fps;
    timer_.cancel();
    if (fps_ != 0) {
        timer_.expires_after(std::chrono::milliseconds(1000 / fps_));
        timer_.async_wait(boost::beast::bind_front_handler(
            &EngineWebSocketSession::on_timer, shared_from_this()));
    }
}

void EngineWebSocketSession::on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        if (ec == boost::beast::websocket::error::closed) {
            return;
        }
        fail(ec, "read");
        return;
    }
    const std::string message = boost::beast::buffers_to_string(buffer_.data());
    buffer_.consume(buffer_.size());
    logger::debug("Received message: {}", message);

    if (message == "get") {
        send_frame();
    }
    if (message.starts_with("set_fps")) {
        auto pos = message.find(' ');
        if (pos != std::string::npos) {
            set_fps(std::stoi(message.substr(pos + 1)));
        }
    }
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
        timer_.async_wait(boost::beast::bind_front_handler(
            &EngineWebSocketSession::on_timer, shared_from_this()));
    }
}

void EngineWebSocketSession::send_frame() {
    auto engine_with_mutex = EngineManager::get_instance().get_engine_with_mutex(engine_name_);
    std::lock_guard lock(engine_with_mutex->mutex);
    // 发送帧
    if (write_in_progress_) {
        logger::warn("Write in progress, frame dropped");
        return;
    }
    frame_buffer_ = engine_with_mutex->engine.get_frame_buffer();
    write_in_progress_ = true;
    ws_.async_write(boost::asio::buffer(frame_buffer_),
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
