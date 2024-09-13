//
// Created by Autumn Sound on 2024/9/5.
//

#include <boost/json.hpp>
#include "handle_request.h"
#include "engine_manger.h"

using RenderCore::Line;

using request = http::request<boost::beast::http::string_body>;
using response = http::response<boost::beast::http::string_body>;


void handle_engine_create(request req, response &res) {
    if (req.method() != http::verb::post) {
        res.result(http::status::bad_request);
        res.set(http::field::server, "RenderEngine");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "The request method must be POST.";
        res.prepare_payload();
        return;
    }
    // Parse the request body
    auto body = req.body();
    auto j = boost::json::parse(body);
    if (!j.is_object()) {
        res.result(http::status::bad_request);
        res.set(http::field::server, "RenderEngine");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "The request body must be a JSON object.";
        res.prepare_payload();
        return;
    }
    const std::string engine_name = "default";
    auto width = j.at("width").as_int64();
    auto height = j.at("height").as_int64();
    EngineManager::get_instance().create_engine(engine_name, width, height);
    res.result(http::status::ok);
    res.set(http::field::server, "RenderEngine");
    res.set(http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());
    res.body() = "Engine created.";
    res.prepare_payload();
}

void handle_request(request req, response &res) {
    if (req.target().starts_with("/engine")) {
        if (req.target().starts_with("/engine/create")) {
            handle_engine_create(req, res);
        } else {
            res.result(http::status::not_found);
            res.set(http::field::server, "RenderEngine");
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + std::string(req.target()) + "' was not found.";
            res.prepare_payload();
        }
    } else {
        res.result(http::status::not_found);
        res.set(http::field::server, "RenderEngine");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(req.target()) + "' was not found.";
        res.prepare_payload();
    }
}