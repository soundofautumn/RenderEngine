//
// Created by Autumn Sound on 2024/9/5.
//
#include <iostream>
#include "Server.h"

void fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}
