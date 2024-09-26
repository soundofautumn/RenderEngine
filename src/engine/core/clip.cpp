//
// Created by Autumn Sound on 2024/9/20.
//
#include <variant>

#include "config.hpp"
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
    return [&](float x, float y) -> uint8_t {
        uint8_t code = INSIDE;
        if (x < window.min_x()) {
            code |= LEFT;
        } else if (x > window.max_x()) {
            code |= RIGHT;
        }
        if (y < window.min_y()) {
            code |= BOTTOM;
        } else if (y > window.max_y()) {
            code |= TOP;
        }
        return code;
    };
};

bool RenderEngine::clip_line_cohen_sutherland(const Rectangle &window, Point &start, Point &end) {
    auto encode = encode_f(window);

    float x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;

    uint8_t start_code = encode(x1, y1);
    uint8_t end_code = encode(x2, y2);

    while (true) {
        // 全部在窗口内
        if (start_code == INSIDE && end_code == INSIDE) {
            start = {static_cast<int>(x1), static_cast<int>(y1)};
            end = {static_cast<int>(x2), static_cast<int>(y2)};
            return true;
        }

        // 全部在窗口外
        if ((start_code & end_code)) {
            return false;
        }

        uint8_t code = start_code ? start_code : end_code;

        float x, y;

        if (code & TOP) {
            x = x1 + (x2 - x1) * (window.max_y() - y1) / (y2 - y1);
            y = window.max_y();
        } else if (code & BOTTOM) {
            x = x1 + (x2 - x1) * (window.min_y() - y1) / (y2 - y1);
            y = window.min_y();
        } else if (code & RIGHT) {
            y = y1 + (y2 - y1) * (window.max_x() - x1) / (x2 - x1);
            x = window.max_x();
        } else if (code & LEFT) {
            y = y1 + (y2 - y1) * (window.min_x() - x1) / (x2 - x1);
            x = window.min_x();
        }

        if (code == start_code) {
            x1 = x;
            y1 = y;
            start_code = encode(x1, y1);
        } else {
            x2 = x;
            y2 = y;
            end_code = encode(x2, y2);
        }
    }
}

bool RenderEngine::clip_line_midpoint(const Rectangle &window, Point &start, Point &end) {
    auto encode = encode_f(window);

    std::function<Point(float, float, float, float)> nearest_point = [&](float x1, float y1,
                                                                         float x2, float y2) {
        if (near_equal(x1, x2, 0.01f) && near_equal(y1, y2, 0.01f)) {
            return Point{static_cast<int>(x1), static_cast<int>(y1)};
        }
        auto start_code = encode(x1, y1);

        float x = (x1 + x2) / 2, y = (y1 + y2) / 2;

        auto mid_code = encode(x, y);

        if (start_code & mid_code) {
            return nearest_point(x, y, x2, y2);
        } else {
            return nearest_point(x1, y1, x, y);
        }
    };

    start = nearest_point(start.x, start.y, end.x, end.y);
    end = nearest_point(end.x, end.y, start.x, start.y);

    return start != end;
}

void RenderEngine::polygon_clip(const Polygon &) {
    RENDERENGINE_UNIMPLEMENTED
    return;
}
