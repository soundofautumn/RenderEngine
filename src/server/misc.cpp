//
// Created by Autumn Sound on 2024/9/5.
//
#include "Server.h"

void fail(boost::system::error_code ec, char const* what) {
    logger::error("{}: {}", what, ec.message());
}
