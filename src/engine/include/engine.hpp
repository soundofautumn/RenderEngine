//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_ENGINE_HPP
#define RENDERENGINE_ENGINE_HPP

#include <deque>
#include <functional>
#include <list>
#include <memory>

#include "bitmap.hpp"
#include "options.hpp"
#include "point.hpp"
#include "polygon.hpp"
#include "primitive.hpp"

namespace RenderCore {
class RenderEngine;
}

class RenderCore::RenderEngine {
    std::unique_ptr<Bitmap> frame_buffer_;

    std::deque<Primitive> primitives_;

    // 存储需要渲染的图元
    std::list<Primitive> render_primitives_;

    int32_t width_;
    int32_t height_;

    PenOptions pen_options_;
    GlobalOptions global_options_;

    // 需要重新渲染
    bool need_render_{true};

   public:
    using Buffer = Bitmap::Buffer;

   public:
    RenderEngine() : frame_buffer_(nullptr), width_(0), height_(0) {}

    RenderEngine(int32_t width, int32_t height)
        : frame_buffer_(nullptr), width_(width), height_(height) {
        init(width, height);
    }

    // 初始化
    void init() { init(width_, height_); }

    // 以指定宽高初始化
    void init(int32_t width, int32_t height) {
        width_ = width;
        height_ = height;
        frame_buffer_ = std::make_unique<Bitmap>(width, height);
        clear();
    }

    // 清空画布，以背景色填充
    void clear() {
        const auto color = vector_to_color(global_options_.background_color);
        if (frame_buffer_) {
            frame_buffer_->fill(color);
        }
    }

    void set_global_options(const GlobalOptions &options) {
        global_options_ = options;
        need_render_ = true;
    }

    [[nodiscard]] const GlobalOptions &get_global_options() const { return global_options_; }

    // 绘制像素
    void draw_pixel(int x, int y, const Color &color) {
        if (!frame_buffer_) {
            return;
        }
        // 忽略超出范围的点
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;
        }
        // 颜色混合
        const auto bg_color = color_to_vector(frame_buffer_->get_pixel(x, y));
        const auto alpha = color.a;
        const auto new_color = color * alpha + bg_color * (1 - alpha);

        frame_buffer_->set_pixel(x, y, vector_to_color(new_color));
    }

    void draw_pixel(int x, int y, uint32_t color) { draw_pixel(x, y, color_to_vector(color)); }

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
        need_render_ = true;
        primitives_.push_back(primitive);
    }

    // 获取图元
    [[nodiscard]] std::vector<std::reference_wrapper<const Primitive>> get_primitives() const {
        return {primitives_.begin(), primitives_.end()};
    }

    // 设置画笔选项
    void set_pen_options(const PenOptions &options) { add_primitive(options); }

    // 获取画笔选项
    [[nodiscard]] const PenOptions &get_pen_options() const { return pen_options_; }

    // 渲染
    bool render() {
        if (!frame_buffer_) {
            return false;
        }
        if (!need_render_) {
            return true;
        }
        clear();
        render_primitives_ = std::list<Primitive>(primitives_.begin(), primitives_.end());
        // 裁剪
        clip();
        pen_options_ = {};
        // 遍历绘制图元
        for (const auto &primitive : render_primitives_) {
            if (std::holds_alternative<Line>(primitive)) {
                const auto &line = std::get<Line>(primitive);
                draw_line(line);
            } else if (std::holds_alternative<Circle>(primitive)) {
                const auto &circle = std::get<Circle>(primitive);
                draw_circle(circle);
            } else if (std::holds_alternative<Arc>(primitive)) {
                const auto &arc = std::get<Arc>(primitive);
                draw_arc(arc);
            } else if (std::holds_alternative<Rectangle>(primitive)) {
                const auto &rectangle = std::get<Rectangle>(primitive);
                draw_rectangle(rectangle);
            } else if (std::holds_alternative<Polygon>(primitive)) {
                const auto &polygon = std::get<Polygon>(primitive);
                draw_polygon(polygon);
            } else if (std::holds_alternative<Fill>(primitive)) {
                const auto &fill = std::get<Fill>(primitive);
                draw_fill(fill);
            } else if (std::holds_alternative<PenOptions>(primitive)) {
                const auto &options = std::get<PenOptions>(primitive);
                pen_options_ = options;
            }
        }
        need_render_ = false;
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

    // 绘制矩形
    void draw_rectangle(const Rectangle &rectangle);

    // 绘制多边形
    void draw_polygon(const Polygon &polygon);

    // 填充
    void draw_fill(const Fill &fill);

    // 裁剪
    void clip();

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

    // 扫描线算法绘制多边形
    void draw_polygon_scanline(const Polygon &polygon);

    // 种子填充算法填充多边形
    void fill_polygon_seedfill(const Fill &fill);

    // 矩形窗口裁剪
    void rectangle_clip(const Rectangle &window);

    // 任意凸多边形裁剪
    void polygon_clip(const Polygon &window);

    // Cohen-Sutherland 裁剪算法
    bool clip_line_cohen_sutherland(const Rectangle &window, Point &start, Point &end);

    // 中点分割裁剪算法
    bool clip_line_midpoint(const Rectangle &window, Point &start, Point &end);

    // 裁剪多边形
    // Sutherland-Hodgman
    bool clip_sutherland_hodgman(const Polygon &window, Polygon &polygon);
};

#endif  //RENDERENGINE_ENGINE_HPP
