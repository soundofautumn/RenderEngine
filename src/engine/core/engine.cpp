//
// Created by Autumn Sound on 2024/9/5.
//

#include "engine.hpp"

#include "circle.hpp"
#include "line.hpp"

using namespace RenderCore;

void RenderEngine::add_primitive(const Primitive &primitive) {
    if (!frame_buffer_) {
        return;
    }
    primitives_.push_back(primitive);
}

bool RenderEngine::render() {
    if (!frame_buffer_) {
        return false;
    }
    clear();
    for (const auto &primitive : primitives_) {
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
    return true;
}
