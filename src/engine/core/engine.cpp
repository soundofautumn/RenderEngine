//
// Created by Autumn Sound on 2024/9/5.
//

#include "line.h"
#include "circle.h"
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
    clear();
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
    // 忽略超出范围的点
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return;
    }
    frame_buffer_->set_pixel(x, y, vector_to_color(color));
}

void RenderEngine::draw_point(int x, int y, const PenOptions &options) {
    draw_pixel(x, y, options.color);
}

void RenderEngine::draw_line(const Point &p1, const Point &p2, const PenOptions &options, LineAlgorithm algorithm) {
    if (!frame_buffer_) {
        return;
    }
    switch (algorithm) {
        case LineAlgorithm::DDA:
            draw_line_by_dda(this, p1, p2, options);
            break;
        case LineAlgorithm::MIDPOINT:
            draw_line_by_midpoint(this, p1, p2, options);
            break;
        case LineAlgorithm::BRESENHAM:
            draw_line_by_bresenham(this, p1, p2, options);
            break;
        default:
            throw std::runtime_error("Unknown line drawing algorithm");
    }
}

void RenderEngine::draw_circle(const Point &center, float radius, const PenOptions &options) {
    if (!frame_buffer_) {
        return;
    }
    draw_circle_by_midpoint(this, center, radius, options);
}

void RenderEngine::draw_arc(const Point &center, float radius, float start_angle, float end_angle,
                            const PenOptions &options) {
    if (!frame_buffer_) {
        return;
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

RenderEngine::Buffer RenderEngine::get_frame_buffer() const {
    if (frame_buffer_) {
        return frame_buffer_->save_to_buffer();
    }
    return {};
}
