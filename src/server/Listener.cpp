//
// Created by Autumn Sound on 2024/9/5.
//

#include "Listener.h"

#include "HttpSession.h"

extern void fail(boost::system::error_code ec, char const *what);

namespace net = boost::beast::net;
namespace logger = spdlog;

Listener::Listener(net::io_context &ioc, const tcp::endpoint &endpoint)
    : acceptor_(ioc), socket_(ioc) {
    boost::system::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}

void Listener::run() {
    if (!acceptor_.is_open()) return;
    do_accept();
}

void Listener::do_accept() {
    acceptor_.async_accept(
        socket_, boost::beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::on_accept(boost::system::error_code ec) {
    if (ec) {
        fail(ec, "accept");
    } else {
        logger::debug("New connection from {}:{}", socket_.remote_endpoint().address().to_string(),
            socket_.remote_endpoint().port());
        // Create the session and run it
        std::make_shared<HttpSession>(std::move(socket_))->run();
    }

    // Accept another connection
    do_accept();
}
