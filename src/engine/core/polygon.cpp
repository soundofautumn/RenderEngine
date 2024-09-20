//
// Created by Autumn Sound on 2024/9/20.
//
#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_rectangle(const Rectangle &rectangle) {
    throw std::runtime_error("Not implemented");
}

void RenderEngine::draw_polygon(const Polygon &polygon) {
    draw_polygon_scanline(polygon);
}

void RenderEngine::draw_polygon_scanline(const Polygon &polygon) {
    throw std::runtime_error("Not implemented");
}
