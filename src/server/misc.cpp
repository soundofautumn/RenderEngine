//
// Created by Autumn Sound on 2024/9/5.
//
#include <spdlog/spdlog.h>

#include <boost/system.hpp>

namespace logger = spdlog;

void fail(boost::system::error_code ec, char const* what) {
    logger::error("{}: {}", what, ec.message());
}
