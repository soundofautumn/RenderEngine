//
// Created by Autumn Sound on 2024/9/9.
//

#ifndef RENDERENGINE_HANDLE_REQUEST_H
#define RENDERENGINE_HANDLE_REQUEST_H

#include <boost/beast.hpp>

namespace http = boost::beast::http;

void handle_request(
    const http::request<http::string_body>& req, http::response<http::string_body>& res);

#endif  //RENDERENGINE_HANDLE_REQUEST_H
