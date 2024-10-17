//
// Created by Autumn Sound on 2024/9/20.
//
#include <variant>
#include <vector>

#include "engine.hpp"
#include "polygon.hpp"
#include "rectangle.hpp"
#include "utils.hpp"
#include "vector.hpp"

using namespace RenderCore;

void RenderEngine::clip(Primitive &primitive) {
    const auto &clip = global_options_.clip;
    if (!clip.enable) {
        return;
    }
    const auto &clip_window = clip.window;
    if (std::holds_alternative<Rectangle>(clip_window)) {
        rectangle_clip(std::get<Rectangle>(clip_window), primitive);
    } else if (std::holds_alternative<Polygon>(clip_window)) {
        polygon_clip(std::get<Polygon>(clip_window), primitive);
    }
}

void RenderEngine::rectangle_clip(const Rectangle &window, Primitive &primitive) {
    if (std::holds_alternative<Line>(primitive)) {
        // 裁剪线段
        auto &line = std::get<Line>(primitive);
        std::optional<Line> new_line = line;
        switch (global_options_.clip.algorithm) {
            default:
#ifdef RENDERENGINE_DEBUG
                throw std::runtime_error("Unknown clip algorithm");
#else
// 默认使用 Cohen-Sutherland 裁剪算法
#endif
            case Clip::Algorithm::CohenSutherland:
                // Cohen-Sutherland 裁剪算法
                clip_line_cohen_sutherland(window, new_line);
                break;
            case Clip::Algorithm::Midpoint:
                // Midpoint 裁剪算法
                clip_line_midpoint(window, new_line);
                break;
        }
        new_line.has_value() ? primitive = new_line.value() : primitive = std::monostate{};
    } else if (std::holds_alternative<Rectangle>(primitive)) {
        // 裁剪矩形
        auto &rectangle = std::get<Rectangle>(primitive);
        std::optional<Polygon> new_polygon = cast_rectangle_to_polygon(rectangle);
        // 使用 Sutherland-Hodgman 算法裁剪多边形
        clip_sutherland_hodgman(cast_rectangle_to_polygon(window), new_polygon);
        new_polygon.has_value() ? primitive = new_polygon.value() : primitive = std::monostate{};
    } else if (std::holds_alternative<Polygon>(primitive)) {
        // 裁剪多边形
        auto &polygon = std::get<Polygon>(primitive);
        std::optional<Polygon> new_polygon = polygon;
        // 使用 Sutherland-Hodgman 算法裁剪多边形
        clip_sutherland_hodgman(cast_rectangle_to_polygon(window), new_polygon);
        new_polygon.has_value() ? primitive = new_polygon.value() : primitive = std::monostate{};
    }
}

constexpr uint8_t INSIDE = 0;  // 0000
constexpr uint8_t LEFT = 1;    // 0001
constexpr uint8_t RIGHT = 2;   // 0010
constexpr uint8_t BOTTOM = 4;  // 0100
constexpr uint8_t TOP = 8;     // 1000

const auto encode_f = [](const Rectangle &window) {
    return [&](float x, float y) -> uint8_t {
        uint8_t code = INSIDE;
        if (x < window.min_x()) {
            code |= LEFT;
        } else if (x > window.max_x()) {
            code |= RIGHT;
        }
        if (y < window.min_y()) {
            code |= BOTTOM;
        } else if (y > window.max_y()) {
            code |= TOP;
        }
        return code;
    };
};

void RenderEngine::clip_line_cohen_sutherland(const Rectangle &window, std::optional<Line> &line) {
    const auto encode = encode_f(window);
    auto &start = line->p1;
    auto &end = line->p2;

    float x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;

    uint8_t start_code = encode(x1, y1);
    uint8_t end_code = encode(x2, y2);

    while (true) {
        // 全部在窗口内
        if (start_code == INSIDE && end_code == INSIDE) {
            start = {static_cast<int>(x1), static_cast<int>(y1)};
            end = {static_cast<int>(x2), static_cast<int>(y2)};
            return;
        }

        // 全部在窗口外
        if ((start_code & end_code)) {
            line.reset();
            return;
        }

        uint8_t code = start_code ? start_code : end_code;

        float x, y;

        if (code & TOP) {
            x = x1 + (x2 - x1) * (window.max_y() - y1) / (y2 - y1);
            y = window.max_y();
        } else if (code & BOTTOM) {
            x = x1 + (x2 - x1) * (window.min_y() - y1) / (y2 - y1);
            y = window.min_y();
        } else if (code & RIGHT) {
            y = y1 + (y2 - y1) * (window.max_x() - x1) / (x2 - x1);
            x = window.max_x();
        } else if (code & LEFT) {
            y = y1 + (y2 - y1) * (window.min_x() - x1) / (x2 - x1);
            x = window.min_x();
        }

        if (code == start_code) {
            x1 = x;
            y1 = y;
            start_code = encode(x1, y1);
        } else {
            x2 = x;
            y2 = y;
            end_code = encode(x2, y2);
        }
    }
}

void RenderEngine::clip_line_midpoint(const Rectangle &window, std::optional<Line> &line) {
    const auto encode = encode_f(window);
    auto &start = line->p1;
    auto &end = line->p2;

    // 计算最近点
    std::function<Point(float, float, float, float)> nearest_point = [&](float x1, float y1,
                                                                         float x2, float y2) {
        if (near_equal(x1, x2, 0.01f) && near_equal(y1, y2, 0.01f)) {
            return Point{static_cast<int>(x1), static_cast<int>(y1)};
        }
        auto start_code = encode(x1, y1);

        float x = (x1 + x2) / 2, y = (y1 + y2) / 2;

        auto mid_code = encode(x, y);

        if (start_code & mid_code) {
            return nearest_point(x, y, x2, y2);
        } else {
            return nearest_point(x1, y1, x, y);
        }
    };

    start = nearest_point(start.x, start.y, end.x, end.y);
    end = nearest_point(end.x, end.y, start.x, start.y);

    if (start == end) {
        line.reset();
    }
}

void RenderEngine::clip_sutherland_hodgman(const Polygon &window, std::optional<Polygon> &polygon) {
    auto &polygon_ref = polygon.value();
    const auto isInside = [&](const Vector2f &point, const Vector2f &edge_start,
                              const Vector2f &edge_end) {
        // 使用叉积判断点是否在边的左侧
        return vector_cross(edge_end - edge_start, point - edge_start) >= 0;
    };

    const auto computeIntersection = [&](const Vector2f &p1, const Vector2f &p2,
                                         const Vector2f &edge_start, const Vector2f &edge_end) {
        // 计算交点
        Vector2f edge_vec = edge_end - edge_start;
        Vector2f poly_vec = p2 - p1;

        // 计算叉积
        float denom = vector_cross(edge_vec, poly_vec);

        // 检查是否平行
        if (near_equal(denom, 0.0f)) {
            // 平行或重合的情况
            return p1;  // 退化处理，返回 p1
        }

        // 计算 t 参数
        Vector2f start_to_p1 = p1 - edge_start;
        float t = vector_cross(start_to_p1, edge_vec) / denom;

        // 计算交点坐标
        return Vector2f{p1.x + t * poly_vec.x, p1.y + t * poly_vec.y};
    };

    // 将点转换为浮点数
    std::vector<Vector2f> window_f(window.size());
    for (size_t i = 0; i < window.size(); i++) {
        window_f[i] = Vector2f{static_cast<float>(window[i].x), static_cast<float>(window[i].y)};
    }
    std::vector<Vector2f> polygon_f(polygon_ref.size());
    for (size_t i = 0; i < polygon_ref.size(); i++) {
        polygon_f[i] =
            Vector2f{static_cast<float>(polygon_ref[i].x), static_cast<float>(polygon_ref[i].y)};
    }

    // 遍历窗口边界
    for (size_t i = 0; i < window.size(); i++) {
        const Vector2f &edge_start = window_f[i];
        const Vector2f &edge_end = window_f[(i + 1) % window.size()];

        // 对于窗口的每条边，计算裁剪后的多边形
        std::vector<Vector2f> new_polygon;
        for (size_t j = 0; j < polygon_f.size(); j++) {
            const Vector2f &p1 = polygon_f[j];
            const Vector2f &p2 = polygon_f[(j + 1) % polygon_f.size()];

            bool p1_inside = isInside(p1, edge_start, edge_end);
            bool p2_inside = isInside(p2, edge_start, edge_end);

            if (p1_inside && p2_inside) {
                // 两个点都在窗口内
                new_polygon.push_back(p2);
            } else if (p1_inside && !p2_inside) {
                // p1 在内部，p2 在外部
                new_polygon.push_back(computeIntersection(p1, p2, edge_start, edge_end));
            } else if (!p1_inside && p2_inside) {
                // p1 在外部，p2 在内部
                new_polygon.push_back(computeIntersection(p1, p2, edge_start, edge_end));
                new_polygon.push_back(p2);
            }
        }
        polygon_f = new_polygon;
    }

    // 将浮点数转换为整数
    polygon_ref.clear();
    polygon_ref.reserve(polygon_f.size());
    for (const auto &point : polygon_f) {
        polygon_ref.push_back(Point{static_cast<int>(point.x), static_cast<int>(point.y)});
    }
    if (polygon_ref.empty()) {
        polygon.reset();
    }
}

enum class Orientation {
    Clockwise,         // 顺时针
    CounterClockwise,  // 逆时针
    Collinear,         // 共线
};

Orientation polygon_orientation(const Polygon &polygon) {
    if (polygon.size() < 3) {
        return Orientation::Collinear;
    }
    float sum = 0;
    for (size_t i = 0; i < polygon.size(); i++) {
        const auto &p1 = polygon[i];
        const auto &p2 = polygon[(i + 1) % polygon.size()];
        sum += (p2.x - p1.x) * (p2.y + p1.y);
    }
    if (sum > 0) {
        return Orientation::CounterClockwise;
    } else if (sum < 0) {
        return Orientation::Clockwise;
    } else {
        return Orientation::Collinear;
    }
}

void RenderEngine::polygon_clip(const Polygon &window, Primitive &primitive) {
    // 要把裁剪窗口的多边形点的顺序改成顺时针
    Polygon new_window = window;
    if (polygon_orientation(window) == Orientation::CounterClockwise) {
        std::reverse(new_window.begin(), new_window.end());
    }
    if (std::holds_alternative<Line>(primitive)) {
        auto &line = std::get<Line>(primitive);
        std::optional<Polygon> new_polygon = Polygon{line.p1, line.p2};
        clip_sutherland_hodgman(new_window, new_polygon);
        new_polygon.has_value() ? primitive = Line{new_polygon.value()[0], new_polygon.value()[1]}
                                : primitive = std::monostate{};
    } else if (std::holds_alternative<Rectangle>(primitive)) {
        auto &rectangle = std::get<Rectangle>(primitive);
        std::optional<Polygon> new_polygon = cast_rectangle_to_polygon(rectangle);
        clip_sutherland_hodgman(new_window, new_polygon);
        new_polygon.has_value() ? primitive = new_polygon.value() : primitive = std::monostate{};
    } else if (std::holds_alternative<Polygon>(primitive)) {
        auto &polygon = std::get<Polygon>(primitive);
        std::optional<Polygon> new_polygon = polygon;
        clip_sutherland_hodgman(new_window, new_polygon);
        new_polygon.has_value() ? primitive = new_polygon.value() : primitive = std::monostate{};
    }
}
