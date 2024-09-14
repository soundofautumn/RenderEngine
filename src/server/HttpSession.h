//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_HTTPSESSION_H
#define RENDERENGINE_HTTPSESSION_H

#include "Server.h"

class HttpSession : public std::enable_shared_from_this<HttpSession> {
    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
public:
    explicit HttpSession(tcp::socket socket);

    void run();

    void do_read();

    void on_read(boost::system::error_code ec);

    void on_write(boost::system::error_code ec, bool close);

    void do_close();
};


#endif //RENDERENGINE_HTTPSESSION_H
