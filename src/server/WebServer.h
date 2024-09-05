//
// Created by 邱姜铭 on 2024/9/5.
//

#ifndef RENDERENGINE_WEBSERVER_H
#define RENDERENGINE_WEBSERVER_H

#include <uwebsockets/App.h>


class WebServer {
public:
    explicit WebServer(int port);

    void start();

private:
    int port;

    uWS::App app;

    void initHttp();

    void initWebSocket();
};


#endif //RENDERENGINE_WEBSERVER_H
