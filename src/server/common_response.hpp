#include <boost/json.hpp>

#include "Server.h"

using request = http::request<boost::beast::http::string_body>;
using response = http::response<boost::beast::http::string_body>;

void success_response(response &res, const std::string &msg) {
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    boost::json::object j{
        {"status", "success"},
        {"message", msg},
    };
    res.body() = boost::json::serialize(j);
}

void error_response(response &res, http::status status, const std::string &msg) {
    res.result(status);
    res.set(http::field::content_type, "application/json");
    boost::json::object j{
        {"status", "error"},
        {"message", msg},
    };
    res.body() = boost::json::serialize(j);
}

void not_found_response(const request &req, response &res) {
    error_response(res, http::status::not_found, "The resource " + std::string(req.target()) + " not found.");
}
