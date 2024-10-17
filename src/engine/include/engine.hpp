//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_ENGINE_HPP
#define RENDERENGINE_ENGINE_HPP

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "bitmap.hpp"
#include "line.hpp"
#include "matrix.hpp"
#include "options.hpp"
#include "point.hpp"
#include "polygon.hpp"
#include "primitive.hpp"
#include "transform.hpp"
#include "vector.hpp"

namespace RenderCore {
class RenderEngine;
}

class RenderCore::RenderEngine {
    // 帧缓冲区
    // 调用 render() 时，将绘制到此缓冲区
    std::unique_ptr<Bitmap> frame_buffer_;

    // 存储所有图元
    // Primitive 是一个变体类型，可以存储多种图元
    // PenOptions 类型的图元会使接下来的图元使用指定的画笔选项，直到下一个 PenOptions 类型的图元
    // Transform 类型的图元会使接下来的可绘制图元使用指定的变换矩阵，仅对接下来的一个图元有效
    // 多个 Transform 类型的图元会叠加变换矩阵
    // 如：[..., T1, T2, L1, ...] 代表先对 L1 进行 T1 变换，再对结果进行 T2 变换
    std::vector<Primitive> primitives_;

    // 存储需要渲染的图元
    // 渲染时，会先将 primitives_ 中的图元复制到 render_primitives_ 中
    // 然后遍历 render_primitives_ 进行渲染
    // 因为可能对图元进行裁剪等操作，所以需要复制一份
    std::list<Primitive> render_primitives_;

    // 画布宽高
    int32_t width_;
    int32_t height_;

    // 画笔选项
    // 在渲染过程自动更新
    // 方便在渲染过程中获取画笔选项
    PenOptions pen_options_;

    // 全局选项
    // 指定背景色、裁剪窗口等
    GlobalOptions global_options_;

    // 变换矩阵
    // 用于对图元进行变换
    Matrix3f transform_matrix_ = Matrix3f::identity();

    // 需要重新渲染
    // 在添加、修改图元时设置为 true
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
        fill_with_background_color();
    }

    // 清空画布，以背景色填充
    void fill_with_background_color() {
        const auto color = vector_to_color(global_options_.background_color);
        if (frame_buffer_) {
            frame_buffer_->fill(color);
        }
    }

    // 初始化画布
    void clear() {
        primitives_.clear();
        global_options_ = {};
        fill_with_background_color();
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

        // 变换
        // 变换矩阵不是单位矩阵时，对点进行变换
        if (transform_matrix_ != Matrix3f::identity()) {
            const auto point = transform_matrix_ * Vector2f(x, y).xy1();
            x = static_cast<int>(point.x);
            y = static_cast<int>(point.y);

            // 重新检查，防止变换后超出范围
            if (x < 0 || x >= width_ || y < 0 || y >= height_) {
                return;
            }
        }

        // 颜色混合
        const auto bg_color = color_to_vector(frame_buffer_->get_pixel(x, y));
        const auto alpha = color.a;
        const auto new_color = color * alpha + bg_color * (1 - alpha);

        // 将新颜色写入帧缓冲区
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

    // 插入图元
    void insert_primitive(const Primitive &primitive, size_t index) {
        if (!frame_buffer_) {
            return;
        }
        need_render_ = true;
        if (index < primitives_.size()) {
            primitives_.insert(primitives_.begin() + index, primitive);
        } else {
            primitives_.push_back(primitive);
        }
    }

    // 移除图元
    void remove_primitive(size_t index) {
        if (!frame_buffer_) {
            return;
        }
        need_render_ = true;
        if (index < primitives_.size()) {
            primitives_.erase(primitives_.begin() + index);
        }
    }

    // 修改图元
    void modify_primitive(size_t index, const Primitive &primitive) {
        if (!frame_buffer_) {
            return;
        }
        need_render_ = true;
        if (index < primitives_.size()) {
            primitives_[index] = primitive;
        }
    }

    // 获取图元
    [[nodiscard]] std::vector<std::reference_wrapper<const Primitive>> get_primitives() const {
        return {primitives_.begin(), primitives_.end()};
    }

    // 设置画笔选项
    void set_pen_options(const PenOptions &options) { add_primitive(options); }

    // 应用变换矩阵
    // 对于能在栅格化前应用变换矩阵的图元，直接应用变换矩阵
    template <typename T>
    void apply_transform(T &t) {
        if constexpr (can_apply_transform_matrix_v<T>) {
            apply_transform_matrix(t, transform_matrix_);
            transform_matrix_ = Matrix3f::identity();
        }
    }

    // 渲染
    bool render() {
        if (!frame_buffer_) {
            return false;
        }
        if (!need_render_) {
            return true;
        }
        fill_with_background_color();
        // 重置画笔选项
        pen_options_ = {};

        render_primitives_ = std::list<Primitive>(primitives_.begin(), primitives_.end());
        for (auto primitive : render_primitives_) {
            std::visit(
                [this](auto &prim) {
                    using T = std::decay_t<decltype(prim)>;
                    if constexpr (std::is_same_v<T, Transform>) {
                        make_transform(prim);
                    } else {
                        apply_transform(prim);
                    }
                },
                primitive);
            // 裁剪
            clip(primitive);
            // 绘制图元
            std::visit(
                [this](const auto &prim) {
                    using T = std::decay_t<decltype(prim)>;
                    if constexpr (std::is_same_v<T, Line>) {
                        draw_line(prim);
                    } else if constexpr (std::is_same_v<T, Circle>) {
                        draw_circle(prim);
                    } else if constexpr (std::is_same_v<T, Arc>) {
                        draw_arc(prim);
                    } else if constexpr (std::is_same_v<T, Rectangle>) {
                        draw_rectangle(prim);
                    } else if constexpr (std::is_same_v<T, Polygon>) {
                        draw_polygon(prim);
                    } else if constexpr (std::is_same_v<T, Fill>) {
                        draw_fill(prim);
                    } else if constexpr (std::is_same_v<T, PenOptions>) {
                        pen_options_ = prim;
                    } else if constexpr (std::is_same_v<T, BezierCurve>) {
                        draw_bezier_curve(prim);
                    } else if constexpr (std::is_same_v<T, BsplineCurve>) {
                        draw_bspline_curve(prim);
                    };
                    // 重置变换矩阵
                    if constexpr (!std::is_same_v<T, Transform> && !std::is_same_v<T, PenOptions>) {
                        transform_matrix_ = Matrix3f::identity();
                    }
                },
                primitive);
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
    void clip(Primitive &primitive);

    // 变换
    void make_transform(const Transform &transform);

    // 贝塞尔曲线
    void draw_bezier_curve(const BezierCurve &curve);

    // B样条曲线
    void draw_bspline_curve(const BsplineCurve &curve);

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
    void rectangle_clip(const Rectangle &window, Primitive &primitive);

    // 任意凸多边形裁剪
    void polygon_clip(const Polygon &window, Primitive &primitive);

    // Cohen-Sutherland 裁剪算法
    void clip_line_cohen_sutherland(const Rectangle &window, std::optional<Line> &line);

    // 中点分割裁剪算法
    void clip_line_midpoint(const Rectangle &window, std::optional<Line> &line);

    // 裁剪多边形
    // Sutherland-Hodgman
    void clip_sutherland_hodgman(const Polygon &window, std::optional<Polygon> &polygon);
};

#endif  //RENDERENGINE_ENGINE_HPP
