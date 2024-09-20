//
// Created by Autumn Sound on 2024/9/20.
//
#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::fill(const Fill &fill) {
    fill_polygon_seedfill(fill);
}

void RenderEngine::fill_polygon_seedfill(const Fill &fill) {
    throw std::runtime_error("Not implemented");
}
