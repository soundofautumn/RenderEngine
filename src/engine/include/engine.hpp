//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_ENGINE_HPP
#define RENDERENGINE_ENGINE_HPP

#include <deque>

#include "bitmap.hpp"
#include "options.hpp"
#include "point.hpp"
#include "primitive.hpp"

namespace RenderCore {
class RenderEngine;
}

class RenderCore::RenderEngine {
    Bitmap *frame_buffer_;

    std::deque<Primitive> primitives_;

    int32_t width_;
    int32_t height_;

    uint32_t color_background_{vector_to_color(Colors::Black)};

   public:
    using Buffer = Bitmap::Buffer;

   public:
    RenderEngine() : frame_buffer_(nullptr), width_(0), height_(0) {}

    RenderEngine(int32_t width, int32_t height)
        : frame_buffer_(nullptr), width_(width), height_(height) {
        init(width, height);
    }

    virtual ~RenderEngine() { reset(); }

    // 初始化
    void init() { init(width_, height_); }

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
    void set_background_color(const Color &color) { color_background_ = vector_to_color(color); }

    void set_background_color(uint32_t color) { color_background_ = color; }

    // 绘制像素
    void draw_pixel(int x, int y, const Color &color) { draw_pixel(x, y, vector_to_color(color)); }

    void draw_pixel(int x, int y, uint32_t color) {
        if (!frame_buffer_) {
            return;
        }
        // 忽略超出范围的点
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;
        }
        frame_buffer_->set_pixel(x, y, color);
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
    void add_primitive(const Primitive &primitive);

    // 获取图元
    [[nodiscard]] const std::vector<Primitive> get_primitives() const { return std::vector(primitives_.begin(), primitives_.end()); }

    // 渲染
    bool render();

   private:
    // 绘制点
    void draw_point(int x, int y, const PenOptions &options = PenOptions(), int index = -1);

    // 绘制线段
    void draw_line(const Line &line);

    // 绘制圆
    void draw_circle(const Circle &circle);

    // 绘制圆弧
    void draw_arc(const Arc &arc);

   private:
    // DDA 算法绘制线段
    void draw_line_dda(const Point &start, const Point &end, const PenOptions &options);

    // 中点算法绘制线段
    void draw_line_midpoint(const Point &start, const Point &end, const PenOptions &options);

    // Bresenham 算法绘制线段
    void draw_line_bresenham(const Point &start, const Point &end, const PenOptions &options);

    // 中点画圆算法
    void draw_circle_midpoint(const Point &center, int radius, const PenOptions &options);

    // 中点画圆弧算法
    void draw_arc_midpoint(const Point &center, int radius, float start_angle, float end_angle,
        const PenOptions &options);
};

#endif  //RENDERENGINE_ENGINE_HPP
