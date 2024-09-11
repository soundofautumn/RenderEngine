//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_ENGINE_HPP
#define RENDERENGINE_ENGINE_HPP

#include "bitmap.hpp"
#include "options.hpp"
#include "point.hpp"
#include "primitive.hpp"

namespace RenderCore {
    class RenderEngine;
}

class RenderCore::RenderEngine {
    Bitmap *frame_buffer_;

    int32_t width_;
    int32_t height_;

    uint32_t color_background_{vector_to_color(Colors::Black)};
public:

    using Buffer = Bitmap::Buffer;

public:
    RenderEngine() : frame_buffer_(nullptr), width_(0), height_(0) {}

    RenderEngine(int32_t width, int32_t height) :
            frame_buffer_(nullptr),
            width_(width),
            height_(height) {
        init(width, height);
    }

    virtual ~RenderEngine() {
        reset();
    }

    // 初始化
    void init() {
        init(width_, height_);
    }

    // 以指定宽高初始化
    void init(int32_t width, int32_t height) {
        reset();
        width_ = width;
        height_ = height;
        frame_buffer_ = new Bitmap(width, height);
        clear();
    }

    // 清空画布，以背景色填充
    void clear() {
        if (frame_buffer_) {
            frame_buffer_->fill(color_background_);
        }
    }

    // 重置
    void reset() {
        if (frame_buffer_) {
            delete frame_buffer_;
            frame_buffer_ = nullptr;
        }
    }

    // 设置背景色
    void set_background_color(const Color &color) {
        color_background_ = vector_to_color(color);
    }

    // 绘制像素
    void draw_pixel(int x, int y, const Color &color) {
        if (!frame_buffer_) {
            return;
        }
        // 忽略超出范围的点
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;
        }
        frame_buffer_->set_pixel(x, y, vector_to_color(color));
    }

    // 保存到文件
    void save(const std::string &filename) {
        if (frame_buffer_) {
            frame_buffer_->save_bmp(filename.c_str());
        }
    }

    // 获取帧缓冲区
    [[nodiscard]] Buffer get_frame_buffer() const {
        if (frame_buffer_) {
            return frame_buffer_->save_to_buffer();
        }
        return {};
    }

    // 绘制图元
    void draw_primitive(const Primitive &primitive);

    // 渲染
    bool render();

    // 绘制点
    void draw_point(int x, int y, const PenOptions &options = PenOptions(), int index = -1);

    // 绘制线段
    void draw_line(const Line &line);

    // 绘制圆
    void draw_circle(const Circle &circle);

    // 绘制圆弧
    void draw_arc(const Arc &arc);
};


#endif //RENDERENGINE_ENGINE_HPP
