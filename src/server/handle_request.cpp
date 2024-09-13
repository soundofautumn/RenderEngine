//
// Created by Autumn Sound on 2024/9/5.
//
#include "handle_request.h"
#include "engine_manger.h"

using RenderCore::Line;

void handle_request(http::request<boost::beast::http::string_body> req,
                    http::response<boost::beast::http::string_body> &res) {
    auto message = req.body();
    if (message.empty()) {
        return;
    }
    auto engine = EngineManager::get_instance().get_engine("default");
    handle_engine_request(message, *engine);
    res.set(http::field::content_type, "text/plain");
    res.result(http::status::ok);
    res.body() = "OK";
}

void clear_canvas(const std::string &message, RenderEngine &engine) {
    if (message == "clear") {
        engine.clear();
    }
}

void set_canvas(const std::string &message, RenderEngine &engine) {
    if (message.starts_with("set_canvas_size")) {
        auto pos = message.find(' ');
        if (pos == std::string::npos) {
            return;
        }
        auto pos2 = message.find(' ', pos + 1);
        if (pos2 == std::string::npos) {
            return;
        }
        auto width = std::stoi(message.substr(pos + 1, pos2 - pos - 1));
        auto height = std::stoi(message.substr(pos2 + 1));
        engine.init(width, height);
        engine.clear();
    }
}

void draw_line(const std::string &message, RenderEngine &engine) {
    if (message.starts_with("draw_line")) {
        const auto pos = message.find(' ');
        if (pos == std::string::npos) {
            return;
        }
        const auto pos2 = message.find(' ', pos + 1);
        if (pos2 == std::string::npos) {
            return;
        }
        const auto pos3 = message.find(' ', pos2 + 1);
        if (pos3 == std::string::npos) {
            return;
        }
        const auto pos4 = message.find(' ', pos3 + 1);
        if (pos4 == std::string::npos) {
            return;
        }
        const auto x1 = std::stoi(message.substr(pos + 1, pos2 - pos - 1));
        const auto y1 = std::stoi(message.substr(pos2 + 1, pos3 - pos2 - 1));
        const auto x2 = std::stoi(message.substr(pos3 + 1, pos4 - pos3 - 1));
        const auto y2 = std::stoi(message.substr(pos4 + 1));
        engine.draw_line({{x1, y1}, {x2, y2}, {.color = RenderCore::Colors::White}, Line::LineAlgorithm::DDA});
    }
}

void draw_point(const std::string &message, RenderEngine &engine) {
    if (message.starts_with("draw_point")) {
        const auto pos = message.find(' ');
        if (pos == std::string::npos) {
            return;
        }
        const auto pos2 = message.find(' ', pos + 1);
        if (pos2 == std::string::npos) {
            return;
        }
        const auto pos3 = message.find(' ', pos2 + 1);
        if (pos3 == std::string::npos) {
            return;
        }
        const auto x = std::stoi(message.substr(pos + 1, pos2 - pos - 1));
        const auto y = std::stoi(message.substr(pos2 + 1, pos3 - pos2 - 1));
        engine.draw_point(x, y, {.color = RenderCore::Colors::White});
    }
}

void handle_engine_request(const std::string &message, RenderEngine &engine) {
    clear_canvas(message, engine);
    set_canvas(message, engine);
    draw_line(message, engine);
    draw_point(message, engine);
}