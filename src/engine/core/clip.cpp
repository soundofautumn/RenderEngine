//
// Created by Autumn Sound on 2024/9/20.
//
#include <variant>

#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::clip() {
    const auto &clip = global_options_.clip;
    if (!clip.enable) {
        return;
    }
    const auto &clip_window = clip.window;
    if (std::holds_alternative<Rectangle>(clip_window)) {
        rectangle_clip(std::get<Rectangle>(clip_window));
    } else if (std::holds_alternative<Polygon>(clip_window)) {
        polygon_clip(std::get<Polygon>(clip_window));
    }
}

void RenderEngine::rectangle_clip(const Rectangle &window) {
    throw std::runtime_error("Not implemented");
}

void RenderEngine::polygon_clip(const Polygon &) {
    throw std::runtime_error("Not implemented");
}
