//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_SERVER_H
#define RENDERENGINE_SERVER_H

#include <spdlog/spdlog.h>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace logger = spdlog;

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;            // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>

constexpr const char SERVER_NAME[] = "RenderEngine";
constexpr const char ENGINE_NAME_HEADER[] = "Engine-Name";
constexpr const char ENGINE_ALLOW_ORIGIN[] = "*";
constexpr const char ENGINE_ALLOW_METHODS[] = "GET, POST, OPTIONS";
constexpr const char ENGINE_ALLOW_HEADERS[] = "Content-Type, Engine-Name";
constexpr const char ENGINE_MAX_AGE[] = "86400";

#endif  //RENDERENGINE_SERVER_H
