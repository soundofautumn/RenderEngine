//
// Created by Autumn Sound on 2024/9/6.
//

#include <cmath>
#include "line.h"
#include "utils.hpp"

// 画特殊斜率的线段
bool draw_line_special_slope(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    if (p1 == p2) {
        engine->draw_pixel(p1.x, p1.y, options.color);
        return true;
    } else if (p1.x == p2.x) {
        int y1 = min(p1.y, p2.y);
        int y2 = max(p1.y, p2.y);
        int x = p1.x;
        for (int y = y1; y <= y2; ++y) {
            engine->draw_pixel(x, y, options.color);
        }
        return true;
    } else if (p1.y == p2.y) {
        int x1 = min(p1.x, p2.x);
        int x2 = max(p1.x, p2.x);
        int y = p1.y;
        for (int x = x1; x <= x2; ++x) {
            engine->draw_pixel(x, y, options.color);
        }
        return true;
    } else {
        return false;
    }
}

void draw_line_by_dda(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    if (draw_line_special_slope(engine, p1, p2, options)) {
        return;
    }
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    float k = (float) (y2 - y1) / (float) (x2 - x1);
    if (abs(k) <= 1) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        auto y = (float) y1;
        for (int x = x1; x <= x2; ++x) {
            engine->draw_pixel(x, round(y), options.color);
            y += k;
        }
    } else {
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        k = 1 / k;
        auto x = (float) x1;
        for (int y = y1; y <= y2; ++y) {
            engine->draw_pixel(round(x), y, options.color);
            x += k;
        }
    }
}

void draw_line_by_midpoint(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    throw std::runtime_error("Not implemented yet");
}

void draw_line_by_bresenham(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    throw std::runtime_error("Not implemented yet");
}
