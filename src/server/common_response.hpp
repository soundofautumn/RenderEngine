#include <boost/beast.hpp>
#include <boost/json.hpp>

namespace http = boost::beast::http;

using request = http::request<http::string_body>;
using response = http::response<http::string_body>;

inline void success_response(response &res, const std::string &msg) {
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    boost::json::object j{
        {"status", "success"},
        {"message", msg},
    };
    res.body() = boost::json::serialize(j);
}

inline void success_response(response &res, const std::string &msg, const boost::json::value &data) {
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    boost::json::object j{
        {"status", "success"},
        {"message", msg},
        {"data", data},
    };
    res.body() = boost::json::serialize(j);
}

inline void error_response(response &res, http::status status, const std::string &msg) {
    res.result(status);
    res.set(http::field::content_type, "application/json");
    boost::json::object j{
        {"status", "error"},
        {"message", msg},
    };
    res.body() = boost::json::serialize(j);
}

inline void not_found_response(const request &req, response &res) {
    error_response(
        res, http::status::not_found, "The resource " + std::string(req.target()) + " not found.");
}
