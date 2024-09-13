//
// Created by Autumn Sound on 2024/9/5.
//

#include <boost/json.hpp>
#include "handle_request.h"
#include "engine_manger.h"

using RenderCore::Line;

using request = http::request<boost::beast::http::string_body>;
using response = http::response<boost::beast::http::string_body>;

void not_found_response(const request& req, response &res) {
    res.result(http::status::not_found);
    res.set(http::field::server, "RenderEngine");
    res.set(http::field::content_type, "text/plain");
    res.body() = "The resource '" + std::string(req.target()) + "' was not found.";
    res.prepare_payload();
}


void handle_engine_create(request req, response &res) {
    if (req.method() != http::verb::post) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "text/plain");
        res.body() = "The request method must be POST.";
        res.prepare_payload();
        return;
    }
    // Parse the request body
    auto body = req.body();
    auto j = boost::json::parse(body);
    if (!j.is_object()) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "text/plain");
        res.body() = "The request body must be a JSON object.";
        res.prepare_payload();
        return;
    }
    auto engine_name = j.at("name").as_string().c_str();
    if (EngineManager::get_instance().check_engine(engine_name)) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "text/plain");
        res.body() = "Engine already exists.";
        res.prepare_payload();
        return;
    }
    auto width = j.at("width").as_int64();
    auto height = j.at("height").as_int64();
    EngineManager::get_instance().create_engine(engine_name, width, height);
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Engine created.";
    res.prepare_payload();
}

void handle_request(const request &req, response &res) {
    res.version(req.version());
    res.set(http::field::server, "RenderEngine");
    res.keep_alive(req.keep_alive());
    try {
        if (req.target().starts_with("/engine")) {
            if (req.target().starts_with("/engine/create")) {
                handle_engine_create(req, res);
            } else {
                return not_found_response(req, res);
            }
        } else {
            return not_found_response(req, res);
        }
    } catch (const std::exception &e) {
        res.result(http::status::internal_server_error);
        res.set(http::field::content_type, "text/plain");
#ifdef NDEBUG
        res.body() = "Internal server error.";
#else
        res.body() = "Internal server error: " + std::string(e.what());
#endif
        res.prepare_payload();
    }
}