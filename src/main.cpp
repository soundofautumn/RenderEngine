#include <iostream>
#include "server/WebServer.h"

int main() {
    WebServer webServer(3000, 3001);
    webServer.start();
    return 0;
}
