//
// Created by 邱姜铭 on 2024/9/5.
//
#include "WebServer.h"
#include <iostream>
#include <string>

WebServer::WebServer(int port) : port(port), acceptor(std::make_shared<tcp::acceptor>(ioc)) {}

void WebServer::start() {
    acceptor->open(tcp::v4());
    acceptor->bind(tcp::endpoint(tcp::v4(), port));
    acceptor->listen();
    initHttp();
    initWebSocket();
    ioc.run();
}

void WebServer::initHttp() {
    acceptor->async_accept([this](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            // Handle HTTP request here
            // You can create a new session for handling HTTP requests
            std::cout << "HTTP connection accepted." << std::endl;
        }
    });
}

void WebServer::initWebSocket() {
    acceptor->async_accept([this](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            websocket::stream<tcp::socket> ws(std::move(socket));
            ws.accept();
            std::cout << "WebSocket connection accepted." << std::endl;

            // Handle WebSocket messages here
            // You can create a new session for handling WebSocket connections
        }
    });
}
