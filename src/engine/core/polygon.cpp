//
// Created by Autumn Sound on 2024/9/20.
//
#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_rectangle(const Rectangle &rectangle) {
    // 共享边界的处理
    // 原则：左闭右开，下闭上开。即矩形左边、下边的像素属于矩形。
    for (int x = rectangle.min_x(); x < rectangle.max_x(); x++) {
        for (int y = rectangle.min_y(); y < rectangle.max_y(); y++) {
            draw_pixel(x, y, pen_options_.fill_color);
        }
    }
    // 画边线
    draw_line(make_line({rectangle.min_x(), rectangle.min_y()}, {rectangle.max_x(), rectangle.min_y()}));
    draw_line(make_line({rectangle.min_x(), rectangle.min_y()}, {rectangle.min_x(), rectangle.max_y()}));
    draw_line(make_line({rectangle.max_x(), rectangle.min_y()}, {rectangle.max_x(), rectangle.max_y()}));
    draw_line(make_line({rectangle.min_x(), rectangle.max_y()}, {rectangle.max_x(), rectangle.max_y()}));
}

void RenderEngine::draw_polygon(const Polygon &polygon) {
    draw_polygon_scanline(polygon);
}

void RenderEngine::draw_polygon_scanline(const Polygon &polygon) {
    throw std::runtime_error("Not implemented");
}
