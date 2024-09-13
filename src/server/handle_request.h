//
// Created by Autumn Sound on 2024/9/9.
//

#ifndef RENDERENGINE_HANDLE_REQUEST_H
#define RENDERENGINE_HANDLE_REQUEST_H

#include <string>
#include "Server.h"
#include "engine.hpp"

using RenderCore::RenderEngine;

void handle_request(const http::request<boost::beast::http::string_body>& req,
                    http::response<boost::beast::http::string_body>& res);

#endif //RENDERENGINE_HANDLE_REQUEST_H
