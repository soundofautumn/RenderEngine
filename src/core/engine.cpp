//
// Created by qjming on 2024/9/5.
//

#include "engine.h"

RenderEngine::RenderEngine() : frame_buffer_(nullptr), width_(0), height_(0) {}

RenderEngine::RenderEngine(int32_t width, int32_t height) :
        frame_buffer_(nullptr),
        width_(width),
        height_(height) {
    init(width, height);
}

RenderEngine::~RenderEngine() {
    reset();
}

void RenderEngine::init() {
    init(width_, height_);
}

void RenderEngine::init(int32_t width, int32_t height) {
    reset();
    width_ = width;
    height_ = height;
    frame_buffer_ = new Bitmap(width, height);
}

void RenderEngine::clear() {
    if (frame_buffer_) {
        frame_buffer_->fill(color_background_);
    }
}

void RenderEngine::reset() {
    if (frame_buffer_) {
        delete frame_buffer_;
        frame_buffer_ = nullptr;
    }
}

void RenderEngine::set_background_color(const Color &color) {
    color_background_ = vector_to_color(color);
}

void RenderEngine::draw_pixel(int x, int y, const Color &color) {
    if (!frame_buffer_) {
        return;
    }
    frame_buffer_->set_pixel(x, y, vector_to_color(color));
}

void RenderEngine::draw_line(const Point &p1, const Point &p2, const Color &color) {
    if (!frame_buffer_) {
        return;
    }
    int x1 = static_cast<int>(p1.x);
    int y1 = static_cast<int>(p1.y);
    int x2 = static_cast<int>(p2.x);
    int y2 = static_cast<int>(p2.y);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;

    int err = dx - dy;

    while (true) {
        draw_pixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

bool RenderEngine::render() {
    return frame_buffer_ != nullptr;
}

void RenderEngine::save(const std::string &filename) {
    if (frame_buffer_) {
        frame_buffer_->save_bmp(filename.c_str());
    }
}
