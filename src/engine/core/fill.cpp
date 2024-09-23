//
// Created by Autumn Sound on 2024/9/20.
//
#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::fill(const Fill &fill) {
    fill_polygon_seedfill(fill);
}

void RenderEngine::fill_polygon_seedfill(const Fill &fill) {
    // 种子填充算法
    std::deque<Point> queue;
    queue.push_back(fill.seed);

    const auto border_color = vector_to_color(pen_options_.color);
    const auto fill_color = vector_to_color(pen_options_.fill_color);

    while (!queue.empty()) {
        auto point = queue.front();
        queue.pop_front();

        if (frame_buffer_->get_pixel(point.x, point.y) == border_color) {
            continue;
        }
        if (frame_buffer_->get_pixel(point.x, point.y) == fill_color) {
            continue;
        }

        draw_pixel(point.x, point.y, fill_color);

        if(point.x > 0) {
            queue.push_back({point.x - 1, point.y});
        }
        if(point.x < width_ - 1) {
            queue.push_back({point.x + 1, point.y});
        }
        if(point.y > 0) {
            queue.push_back({point.x, point.y - 1});
        }
        if(point.y < height_ - 1) {
            queue.push_back({point.x, point.y + 1});
        }
    }
}
