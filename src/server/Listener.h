//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_LISTENER_H
#define RENDERENGINE_LISTENER_H

#include "Server.h"

class Listener : public std::enable_shared_from_this<Listener> {
    tcp::acceptor acceptor_;
    tcp::socket socket_;

public:
    Listener(boost::asio::io_context &ioc,
             const tcp::endpoint& endpoint);

    // Start accepting incoming connections
    void run();

    void do_accept();

    void on_accept(boost::system::error_code ec);
};


#endif //RENDERENGINE_LISTENER_H
