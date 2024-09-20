//
// Created by Autumn Sound on 2024/9/20.
//
#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_rectangle(const Rectangle &rectangle) {
    draw_line(Line{{rectangle.min_x, rectangle.min_y}, {rectangle.max_x, rectangle.min_y}});
    draw_line(Line{{rectangle.max_x, rectangle.min_y}, {rectangle.max_x, rectangle.max_y}});
    draw_line(Line{{rectangle.max_x, rectangle.max_y}, {rectangle.min_x, rectangle.max_y}});
    draw_line(Line{{rectangle.min_x, rectangle.max_y}, {rectangle.min_x, rectangle.min_y}});
}

void RenderEngine::fill_rectangle(const Rectangle &rectangle) {
    for (int y = rectangle.min_y; y <= rectangle.max_y; ++y) {
        draw_line(Line{{rectangle.min_x, y}, {rectangle.max_x, y}});
    }
}

void RenderEngine::draw_polygon(const Polygon &polygon) {
    draw_polygon_scanline(polygon);
}

void RenderEngine::fill_polygon(const Polygon &polygon) {
    fill_polygon_seedfill(polygon);
}

void RenderEngine::draw_polygon_scanline(const Polygon &polygon) {
    throw std::runtime_error("Not implemented");
}

void RenderEngine::fill_polygon_seedfill(const Polygon &polygon) {
    throw std::runtime_error("Not implemented");
}
