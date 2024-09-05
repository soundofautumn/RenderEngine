//
// Created by 邱姜铭 on 2024/9/5.
//

#include "WebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

WebSocketSession::WebSocketSession(tcp::socket socket)
        : socket_(std::move(socket)),
          ws_(std::move(socket_)) {
}

void WebSocketSession::do_accept(http::request<http::string_body> req) {
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

    logger::info("WebSocket connection from {}:{}",
                 ws_.next_layer().remote_endpoint().address().to_string(),
                 ws_.next_layer().remote_endpoint().port());

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

    // TODO:

    ws_.text(ws_.got_text());

    if (ws_.got_text()) {
        logger::info("WebSocket message from {}:{}: {}",
                     ws_.next_layer().remote_endpoint().address().to_string(),
                     ws_.next_layer().remote_endpoint().port(),
                     boost::beast::buffers_to_string(buffer_.data()));
    } else {
        logger::info("WebSocket binary message from {}:{}",
                     ws_.next_layer().remote_endpoint().address().to_string(),
                     ws_.next_layer().remote_endpoint().port());
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


