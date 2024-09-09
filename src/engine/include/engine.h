//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_ENGINE_H
#define RENDERENGINE_ENGINE_H

#include "bitmap.hpp"
#include "vector.hpp"
#include "options.hpp"

class RenderEngine {
    Bitmap *frame_buffer_;

    int32_t width_;
    int32_t height_;

    uint32_t color_background_{vector_to_color(Colors::Black)};
public:
    using Color = Vector4f;
    using Point = Vector2i;

    using Buffer = Bitmap::Buffer;

public:
    RenderEngine();

    RenderEngine(int32_t width, int32_t height);

    virtual ~RenderEngine();

    // 初始化
    void init();

    // 以指定宽高初始化
    void init(int32_t width, int32_t height);

    // 清空画布，以背景色填充
    void clear();

    // 重置
    void reset();

    // 设置背景色
    void set_background_color(const Color &color);

    // 渲染
    bool render();

    // 绘制像素
    void draw_pixel(int x, int y, const Color &color);

    // 绘制点
    void draw_point(const Point &p, const PenOptions &options = PenOptions());

    // 绘制线段
    void draw_line(const Point &p1, const Point &p2, const PenOptions &options = PenOptions(),
                   LineAlgorithm algorithm = LineAlgorithm::BRESENHAM);

    // 绘制圆弧
    void draw_circle(const Point &center, float radius, const PenOptions &options = PenOptions());

    void draw_arc(const Point &center, float radius, float start_angle, float end_angle,
                  const PenOptions &options = PenOptions());

    // 保存到文件
    void save(const std::string &filename);

    // 获取文件
    [[nodiscard]] Buffer get_frame_buffer() const;
};


#endif //RENDERENGINE_ENGINE_H
