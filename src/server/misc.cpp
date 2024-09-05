//
// Created by 邱姜铭 on 2024/9/5.
//
#include <iostream>
#include "Server.h"

void fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}
