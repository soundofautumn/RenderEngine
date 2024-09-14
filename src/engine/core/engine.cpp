//
// Created by Autumn Sound on 2024/9/5.
//

#include "engine.hpp"

#include "circle.hpp"
#include "line.hpp"

using namespace RenderCore;

void RenderEngine::draw_primitive(const Primitive &primitive) {
    if (!frame_buffer_) {
        return;
    }
    if (std::holds_alternative<Line>(primitive)) {
        const auto &line = std::get<Line>(primitive);
        draw_line(line);
    } else if (std::holds_alternative<Circle>(primitive)) {
        const auto &circle = std::get<Circle>(primitive);
        draw_circle(circle);
    } else if (std::holds_alternative<Arc>(primitive)) {
        const auto &arc = std::get<Arc>(primitive);
        draw_arc(arc);
    }
}

bool RenderEngine::render() {
    return frame_buffer_ != nullptr;
}