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
    auto it = render_primitives_.begin();
    while (it != render_primitives_.end()) {
        const auto &primitive = *it;
        if (std::holds_alternative<Line>(primitive)) {
            auto &line = std::get<Line>(primitive);
            Line new_line = line;
            bool should_draw = true;
            switch (global_options_.clip.algorithm) {
                case Clip::Algorithm::CohenSutherland:
                    // Cohen-Sutherland 裁剪算法
                    should_draw = clip_line_cohen_sutherland(window, new_line.p1, new_line.p2);
                    break;
                case Clip::Algorithm::Midpoint:
                    // Midpoint 裁剪算法
                    should_draw = clip_line_midpoint(window, new_line.p1, new_line.p2);
                    break;
                default:
                    throw std::runtime_error("Unknown clip algorithm");
                    break;
            }
            if (should_draw) {
                *it = new_line;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        }
        ++it;
    }
}
const uint8_t INSIDE = 0;  // 0000
const uint8_t LEFT = 1;    // 0001
const uint8_t RIGHT = 2;   // 0010
const uint8_t BOTTOM = 4;  // 0100
const uint8_t TOP = 8;     // 1000

auto encode_f = [](const Rectangle &window) {
    return [&](const Point &point) {
        uint8_t code = INSIDE;
        if (point.x < window.min_x()) {
            code |= LEFT;
        } else if (point.x > window.max_x()) {
            code |= RIGHT;
        } else if (point.y < window.min_y()) {
            code |= BOTTOM;
        } else if (point.y > window.max_y()) {
            code |= TOP;
        }
        return code;
    };
};

bool RenderEngine::clip_line_cohen_sutherland(const Rectangle &window, Point &start, Point &end) {
    auto encode = encode_f(window);

    auto start_code = encode(start);
    auto end_code = encode(end);

    if (start_code & end_code) {
        // 全部在窗口外
        return false;
    }

    while (true) {
        if (!(start_code | end_code)) {
            // 全部在窗口内
            return true;
        } else {
            // 部分在窗口内
            Point intersect;
            uint8_t out_code = start_code ? start_code : end_code;
            if (out_code & TOP) {
                intersect.x =
                    start.x + (end.x - start.x) * (window.max_y() - start.y) / (end.y - start.y);
                intersect.y = window.max_y();
            } else if (out_code & BOTTOM) {
                intersect.x =
                    start.x + (end.x - start.x) * (window.min_y() - start.y) / (end.y - start.y);
                intersect.y = window.min_y();
            } else if (out_code & RIGHT) {
                intersect.y =
                    start.y + (end.y - start.y) * (window.max_x() - start.x) / (end.x - start.x);
                intersect.x = window.max_x();
            } else if (out_code & LEFT) {
                intersect.y =
                    start.y + (end.y - start.y) * (window.min_x() - start.x) / (end.x - start.x);
                intersect.x = window.min_x();
            }
            if (out_code == start_code) {
                start = intersect;
                start_code = encode(start);
            } else {
                end = intersect;
                end_code = encode(end);
            }
        }
    }
}

bool RenderEngine::clip_line_midpoint(const Rectangle &window, Point &start, Point &end) {
    throw std::runtime_error("Not implemented");
}

void RenderEngine::polygon_clip(const Polygon &) {
    throw std::runtime_error("Not implemented");
}
