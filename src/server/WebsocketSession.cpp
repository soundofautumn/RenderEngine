//
// Created by 邱姜铭 on 2024/9/5.
//

#include "WebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

WebSocketSession::WebSocketSession(tcp::socket socket)
        : socket_(std::move(socket)),
          ws_(std::move(socket_)) {
}

void WebSocketSession::do_accept(boost::beast::http::request<boost::beast::http::string_body> req) {
    ws_.async_accept(
            req,
            boost::beast::bind_front_handler(
                    &WebSocketSession::on_accept,
                    shared_from_this()));
}

void WebSocketSession::on_accept(boost::system::error_code ec) {
    if (ec) {
        fail(ec, "accept");
        return;
    }

    ws_.text(true);
    ws_.async_read(
            buffer_,
            boost::beast::bind_front_handler(
                    &WebSocketSession::on_read,
                    shared_from_this()));
}

void WebSocketSession::on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        fail(ec, "read");
        return;
    }

    ws_.async_write(
            buffer_.data(),
            boost::beast::bind_front_handler(
                    &WebSocketSession::on_write,
                    shared_from_this()));
}

void WebSocketSession::on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        fail(ec, "write");
        return;
    }

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Do another read
    ws_.async_read(
            buffer_,
            boost::beast::bind_front_handler(
                    &WebSocketSession::on_read,
                    shared_from_this()));
}


