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

    PenOptions pen_options_;

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
    void add_primitive(const Primitive &primitive) {
        if (!frame_buffer_) {
            return;
        }
        primitives_.push_back(primitive);
        if (std::holds_alternative<PenOptions>(primitive)) {
            pen_options_ = std::get<PenOptions>(primitive);
        }
    }

    // 获取图元
    [[nodiscard]] const std::vector<Primitive> get_primitives() const {
        return std::vector(primitives_.begin(), primitives_.end());
    }

    // 设置画笔选项
    void set_pen_options(const PenOptions &options) {
        add_primitive(options);
    }

    // 获取画笔选项
    [[nodiscard]] const PenOptions &get_pen_options() const { return pen_options_; }

    // 渲染
    bool render() {
        if (!frame_buffer_) {
            return false;
        }
        clear();
        for (const auto &primitive : primitives_) {
            if (std::holds_alternative<Line>(primitive)) {
                const auto &line = std::get<Line>(primitive);
                draw_line(line);
            } else if (std::holds_alternative<Circle>(primitive)) {
                const auto &circle = std::get<Circle>(primitive);
                draw_circle(circle);
            } else if (std::holds_alternative<Arc>(primitive)) {
                const auto &arc = std::get<Arc>(primitive);
                draw_arc(arc);
            } else if (std::holds_alternative<PenOptions>(primitive)) {
                const auto &options = std::get<PenOptions>(primitive);
                pen_options_ = options;
            }
        }
        return true;
    }

   private:
    // 绘制点
    void draw_point(int x, int y, int index = -1);

    // 绘制线段
    void draw_line(const Line &line);

    // 绘制圆
    void draw_circle(const Circle &circle);

    // 绘制圆弧
    void draw_arc(const Arc &arc);

   private:
    // DDA 算法绘制线段
    void draw_line_dda(const Point &start, const Point &end);

    // 中点算法绘制线段
    void draw_line_midpoint(const Point &start, const Point &end);

    // Bresenham 算法绘制线段
    void draw_line_bresenham(const Point &start, const Point &end);

    // 中点画圆算法
    void draw_circle_midpoint(const Point &center, int radius);

    // 中点画圆弧算法
    void draw_arc_midpoint(const Point &center, int radius, float start_angle, float end_angle);
};

#endif  //RENDERENGINE_ENGINE_HPP
