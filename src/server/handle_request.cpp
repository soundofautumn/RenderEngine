//
// Created by Autumn Sound on 2024/9/5.
//

#include "handle_request.h"

#include <boost/json/src.hpp>

#include "EngineManager.h"
#include "Server.h"
#include "common_response.hpp"
#include "serialize/serialize.h"
#include "serialize/serialize_options.h"

using RenderCore::Primitive;

using request = http::request<boost::beast::http::string_body>;
using response = http::response<boost::beast::http::string_body>;

boost::json::value get_request_body(const request &req, response &res) {
    if (req.method() != http::verb::post) {
        error_response(res, http::status::bad_request, "The request method must be POST.");
        return {};
    }
    const auto &body = req.body();
    auto j = boost::json::parse(body);
    if (!j.is_object()) {
        error_response(res, http::status::bad_request, "The request body must be a JSON object.");
        return {};
    }
    return j;
}

void handle_engine_create(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine_name = j.at("name").as_string().c_str();
    if (EngineManager::get_instance().check_engine(engine_name)) {
        success_response(res, "Engine already exists.");
        return;
    }
    auto width = static_cast<int>(j.at("width").as_int64());
    auto height = static_cast<int>(j.at("height").as_int64());
    EngineManager::get_instance().create_engine(engine_name, width, height);
    success_response(res, "Engine created.");
}

std::string get_engine_name(const request &req, response &res) {
    if (req.find(ENGINE_NAME_HEADER) == req.end()) {
        error_response(res, http::status::bad_request, "Engine name not found.");
        return {};
    }
    return req[ENGINE_NAME_HEADER];
}

void handle_engine_remove(const request &req, response &res) {
    auto engine_name = get_engine_name(req, res);
    if (engine_name.empty()) {
        return;
    }
    EngineManager::get_instance().remove_engine(engine_name);
    success_response(res, "Engine removed.");
}

std::shared_ptr<EngineManager::EngineMutex> get_engine_with_mutex(
    const request &req, response &res) {
    auto engine_name = get_engine_name(req, res);
    if (engine_name.empty()) {
        return nullptr;
    }
    if (!EngineManager::get_instance().check_engine(engine_name)) {
        error_response(res, http::status::not_found, "Engine not found.");
        return nullptr;
    }
    return EngineManager::get_instance().get_engine_with_mutex(engine_name);
}

void handle_engine_draw(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine_mutex = get_engine_with_mutex(req, res);
    if (!engine_mutex) {
        return;
    }
    try {
        auto primitive = deserialize_primitive(j.as_object());
        logger::trace("Add primitive: {}", boost::json::serialize(j));
        {
            std::lock_guard<std::mutex> lock(engine_mutex->mutex);
            engine_mutex->engine.add_primitive(primitive);
        }
    } catch (const std::exception &e) {
#ifdef NDEBUG
        auto msg = "Invalid primitive.";
#else
        auto msg = "Invalid primitive: " + std::string(e.what());
#endif
        error_response(res, http::status::bad_request, msg);
    }
    success_response(res, "Primitive added.");
}

void handle_engine_get_primitives(const request &req, response &res) {
    auto engine = get_engine_with_mutex(req, res);
    if (!engine) {
        return;
    }
    boost::json::array j_primitives;
    {
        std::lock_guard<std::mutex> lock(engine->mutex);
        auto primitives = engine->engine.get_primitives();
        for (const auto &primitive : primitives) {
            j_primitives.push_back(serialize_primitive(primitive));
        }
    }
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(j_primitives);
}

void handle_engine_set_global_options(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine = get_engine_with_mutex(req, res);
    if (!engine) {
        return;
    }
    if (!j.as_object().contains("GlobalOptions")) {
        error_response(res, http::status::bad_request, "Global options not found.");
        return;
    }
    auto global_options = deserialize_global_options(j.at("GlobalOptions").as_object());
    logger::trace("Set global options: {}", boost::json::serialize(j));
    {
        std::lock_guard<std::mutex> lock(engine->mutex);
        engine->engine.set_global_options(global_options);
    }
    success_response(res, "Global options set.");
}

void handle_engine_insert_primitive(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine = get_engine_with_mutex(req, res);
    if (!engine) {
        return;
    }
    if (!j.as_object().contains("Primitive")) {
        error_response(res, http::status::bad_request, "Primitive not found.");
        return;
    }
    auto primitive = deserialize_primitive(j.at("Primitive").as_object());
    auto index = static_cast<size_t>(j.at("Index").as_int64());
    logger::trace("Insert primitive at {}: {}", index, boost::json::serialize(j));
    {
        std::lock_guard<std::mutex> lock(engine->mutex);
        engine->engine.insert_primitive(primitive, index);
    }
    success_response(res, "Primitive inserted.");
}

void handle_engine_remove_primitive(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine = get_engine_with_mutex(req, res);
    if (!engine) {
        return;
    }
    auto index = static_cast<size_t>(j.at("Index").as_int64());
    logger::trace("Remove primitive at {}: {}", index, boost::json::serialize(j));
    {
        std::lock_guard<std::mutex> lock(engine->mutex);
        engine->engine.remove_primitive(index);
    }
    success_response(res, "Primitive removed.");
}

void handle_engine_modify_primitive(const request &req, response &res) {
    auto j = get_request_body(req, res);
    if (j.is_null()) {
        return;
    }
    auto engine = get_engine_with_mutex(req, res);
    if (!engine) {
        return;
    }
    if (!j.as_object().contains("Primitive")) {
        error_response(res, http::status::bad_request, "Primitive not found.");
        return;
    }
    auto primitive = deserialize_primitive(j.at("Primitive").as_object());
    auto index = static_cast<size_t>(j.at("Index").as_int64());
    logger::trace("Modify primitive at {}: {}", index, boost::json::serialize(j));
    {
        std::lock_guard<std::mutex> lock(engine->mutex);
        engine->engine.modify_primitive(index, primitive);
    }
    success_response(res, "Primitive modified.");
}

void handle_request(const request &req, response &res) {
    res.version(req.version());
    res.set(http::field::server, SERVER_NAME);
    res.set(http::field::access_control_allow_origin, ENGINE_ALLOW_ORIGIN);
    res.set(http::field::access_control_allow_methods, ENGINE_ALLOW_METHODS);
    res.set(http::field::access_control_allow_headers, ENGINE_ALLOW_HEADERS);
    res.set(http::field::access_control_max_age, ENGINE_MAX_AGE);
    res.keep_alive(req.keep_alive());

    if (req.method() == http::verb::options) {
        res.result(http::status::ok);
        res.prepare_payload();
        return;
    }

    try {
        if (req.target().starts_with("/engine")) {
            if (req.target().starts_with("/engine/create")) {
                handle_engine_create(req, res);
            } else if (req.target().starts_with("/engine/remove")) {
                handle_engine_remove(req, res);
            } else if (req.target().starts_with("/engine/primitive/push_back")) {
                handle_engine_draw(req, res);
            } else if (req.target().starts_with("/engine/primitive/insert")) {
                handle_engine_insert_primitive(req, res);
            } else if (req.target().starts_with("/engine/primitive/remove")) {
                handle_engine_remove_primitive(req, res);
            } else if (req.target().starts_with("/engine/primitive/modify")) {
                handle_engine_modify_primitive(req, res);
            } else if (req.target().starts_with("/engine/primitive/get_all")) {
                handle_engine_get_primitives(req, res);
            } else if (req.target().starts_with("/engine/set_global_options")) {
                handle_engine_set_global_options(req, res);
            } else if (req.target().starts_with("/engine/ws")) {
                res.result(http::status::ok);
            } else {
                return not_found_response(req, res);
            }
        } else {
            return not_found_response(req, res);
        }
    } catch (const std::exception &e) {
#ifdef NDEBUG
        auto msg = "Internal server error.";
#else
        auto msg = "Internal server error: " + std::string(e.what());
#endif
        error_response(res, http::status::bad_request, msg);
    }
    res.prepare_payload();
}
