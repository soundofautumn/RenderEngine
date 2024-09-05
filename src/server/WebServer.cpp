//
// Created by 邱姜铭 on 2024/9/5.
//

#include "WebServer.h"

WebServer::WebServer(int port) : port(port) {
    initHttp();
    initWebSocket();
}

void WebServer::initHttp() {
    app.get("/", [](auto *res, auto *req) {
        res->end("Hello, World!");
    });
}

struct PerSocketData {
};

void WebServer::initWebSocket() {
    app.ws<PerSocketData>("/ws", {
        .open = [](auto *ws) {
            std::cout << "WebSocket connected" << std::endl;
        },
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
            ws->send(message, opCode);
        },
        .close = [](auto *ws, int code, std::string_view message) {
            std::cout << "WebSocket closed" << std::endl;
        }
    });
}

void WebServer::start() {
    app.listen(port, [this](auto *token) {
        if (token) {
            std::cout << "Listening on port " << port << std::endl;
        }
    }).run();
}
