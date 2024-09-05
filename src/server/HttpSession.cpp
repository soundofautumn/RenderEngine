//
// Created by 邱姜铭 on 2024/9/5.
//

#include "HttpSession.h"
#include "WebSocketSession.h"

extern void fail(boost::system::error_code ec, char const *what);

HttpSession::HttpSession(tcp::socket socket) : socket_(std::move(socket)) {}

void HttpSession::run() {
    do_read();
}

void HttpSession::on_read(boost::system::error_code ec) {
    if (ec) {
        fail(ec, "read");
        return;
    }

    // See if it is a WebSocket Upgrade
    if (boost::beast::websocket::is_upgrade(req_)) {
        // Create a WebSocket websocket_session by transferring the socket
        std::make_shared<WebSocketSession>(std::move(socket_))->do_accept(std::move(req_));
        return;
    }

    // Send the response
//    handle_request(std::move(req_), res_);

    auto self = shared_from_this();
    boost::beast::http::async_write(
            socket_,
            res_,
            [self, this](boost::system::error_code ec, std::size_t bytes_transferred) {
                self->on_write(ec, res_.need_eof());
            });
}

void HttpSession::on_write(boost::system::error_code ec, bool close) {
    if (ec) {
        fail(ec, "write");
        return;
    }

    if (close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return;
    }

    // We're done with the response so delete it
    res_ = {};

    // Read another request
    do_read();
}

void HttpSession::do_read() {
    // Read a request
    boost::beast::http::async_read(
            socket_,
            buffer_,
            req_,
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                self->on_read(ec);
            });
}

void HttpSession::do_close() {
    // Send a TCP shutdown
    boost::system::error_code ec;
    socket_.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}