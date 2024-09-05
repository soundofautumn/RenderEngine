#include <iostream>
#include "server/WebServer.h"

int main() {
    WebServer webServer(3000);
    webServer.start();
    return 0;
}
