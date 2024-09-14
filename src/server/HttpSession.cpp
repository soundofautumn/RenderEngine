//
// Created by Autumn Sound on 2024/9/5.
//

#include "HttpSession.h"

#include "EngineWebSocketSession.h"
#include "WebSocketSession.h"
#include "handle_request.h"

extern void fail(boost::system::error_code ec, char const *what);

HttpSession::HttpSession(tcp::socket socket) : socket_(std::move(socket)) {}

void HttpSession::run() {
    do_read();
}

void HttpSession::on_read(boost::system::error_code ec) {
    // This means they closed the connection
    if (ec == http::error::end_of_stream) return do_close();

    if (ec) {
        fail(ec, "read");
        return;
    }

    // See if it is a WebSocket Upgrade
    if (websocket::is_upgrade(req_)) {
        logger::debug("WebSocket upgrade request from {}:{}",
            socket_.remote_endpoint().address().to_string(), socket_.remote_endpoint().port());

        if (req_.target().starts_with("/engine/")) {
            // Create a WebSocket engine_session by transferring the socket
            std::make_shared<EngineWebSocketSession>(std::move(socket_))->run(req_);
            return;
        }

        // Create a WebSocket websocket_session by transferring the socket
        std::make_shared<WebSocketSession>(std::move(socket_))->do_accept(req_);
        return;
    }

    logger::debug("HTTP request from {}:{} {}", socket_.remote_endpoint().address().to_string(),
        socket_.remote_endpoint().port(), req_.target());

    handle_request(req_, res_);

    req_ = {};

    http::async_write(
        socket_, res_, [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
            self->on_write(ec, self->res_.need_eof());
        });
}

void HttpSession::on_write(boost::system::error_code ec, bool close) {
    // Happens when the timer closes the socket
    if (ec == net::error::operation_aborted) return;

    if (ec) {
        fail(ec, "write");
        return;
    }

    if (close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
    }

    // We're done with the response so delete it
    res_ = {};

    // Read another request
    do_read();
}

void HttpSession::do_read() {
    // Read a request
    http::async_read(socket_, buffer_, req_,
        [self = shared_from_this()](
            boost::system::error_code ec, std::size_t) { self->on_read(ec); });
}

void HttpSession::do_close() {
    // Send a TCP shutdown
    boost::system::error_code ec;
    socket_.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}