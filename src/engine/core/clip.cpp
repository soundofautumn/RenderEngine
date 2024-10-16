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

void RenderEngine::clip() {
    const auto &clip = global_options_.clip;
    if (!clip.enable) {
        return;
    }
    const auto &clip_window = clip.window;
    if (std::holds_alternative<Rectangle>(clip_window)) {
        rectangle_clip(std::get<Rectangle>(clip_window));
    } else if (std::holds_alternative<Polygon>(clip_window)) {
        polygon_clip(std::get<Polygon>(clip_window));
    }
}

void RenderEngine::rectangle_clip(const Rectangle &window) {
    auto it = render_primitives_.begin();
    while (it != render_primitives_.end()) {
        const auto &primitive = *it;
        if (std::holds_alternative<Line>(primitive)) {
            // 裁剪线段
            auto &line = std::get<Line>(primitive);
            Line new_line = line;
            bool should_draw = true;
            switch (global_options_.clip.algorithm) {
                default:
#ifdef RENDERENGINE_DEBUG
                    throw std::runtime_error("Unknown clip algorithm");
#else
// 默认使用 Cohen-Sutherland 裁剪算法
#endif
                case Clip::Algorithm::CohenSutherland:
                    // Cohen-Sutherland 裁剪算法
                    should_draw = clip_line_cohen_sutherland(window, new_line.p1, new_line.p2);
                    break;
                case Clip::Algorithm::Midpoint:
                    // Midpoint 裁剪算法
                    should_draw = clip_line_midpoint(window, new_line.p1, new_line.p2);
                    break;
            }
            if (should_draw) {
                *it = new_line;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        } else if (std::holds_alternative<Rectangle>(primitive)) {
            // 裁剪矩形
            auto &rectangle = std::get<Rectangle>(primitive);
            Polygon new_polygon = cast_rectangle_to_polygon(rectangle);
            bool should_draw = true;
            // 使用 Sutherland-Hodgman 算法裁剪多边形
            should_draw = clip_sutherland_hodgman(cast_rectangle_to_polygon(window), new_polygon);
            if (should_draw) {
                *it = new_polygon;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        } else if (std::holds_alternative<Polygon>(primitive)) {
            // 裁剪多边形
            auto &polygon = std::get<Polygon>(primitive);
            Polygon new_polygon = polygon;
            bool should_draw = true;
            // 使用 Sutherland-Hodgman 算法裁剪多边形
            should_draw = clip_sutherland_hodgman(cast_rectangle_to_polygon(window), new_polygon);
            if (should_draw) {
                *it = new_polygon;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        }
        ++it;
    }
}

const uint8_t INSIDE = 0;  // 0000
const uint8_t LEFT = 1;    // 0001
const uint8_t RIGHT = 2;   // 0010
const uint8_t BOTTOM = 4;  // 0100
const uint8_t TOP = 8;     // 1000

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

bool RenderEngine::clip_line_cohen_sutherland(const Rectangle &window, Point &start, Point &end) {
    const auto encode = encode_f(window);

    float x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;

    uint8_t start_code = encode(x1, y1);
    uint8_t end_code = encode(x2, y2);

    while (true) {
        // 全部在窗口内
        if (start_code == INSIDE && end_code == INSIDE) {
            start = {static_cast<int>(x1), static_cast<int>(y1)};
            end = {static_cast<int>(x2), static_cast<int>(y2)};
            return true;
        }

        // 全部在窗口外
        if ((start_code & end_code)) {
            return false;
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

bool RenderEngine::clip_line_midpoint(const Rectangle &window, Point &start, Point &end) {
    const auto encode = encode_f(window);

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

    return start != end;
}

bool RenderEngine::clip_sutherland_hodgman(const Polygon &window, Polygon &polygon) {
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
    std::vector<Vector2f> polygon_f(polygon.size());
    for (size_t i = 0; i < polygon.size(); i++) {
        polygon_f[i] = Vector2f{static_cast<float>(polygon[i].x), static_cast<float>(polygon[i].y)};
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
    polygon.clear();
    polygon.reserve(polygon_f.size());
    for (const auto &point : polygon_f) {
        polygon.push_back(Point{static_cast<int>(point.x), static_cast<int>(point.y)});
    }

    return !polygon.empty();  // 如果裁剪后多边形非空，返回 true
}

enum class Orientation {
    Clockwise,
    CounterClockwise,
    Collinear,
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

void RenderEngine::polygon_clip(const Polygon &window) {
    // 要把裁剪窗口的多边形点的顺序改成顺时针
    Polygon new_window = window;
    if (polygon_orientation(window) == Orientation::CounterClockwise) {
        std::reverse(new_window.begin(), new_window.end());
    }
    auto it = render_primitives_.begin();
    while (it != render_primitives_.end()) {
        const auto &primitive = *it;
        if (std::holds_alternative<Line>(primitive)) {
            auto &line = std::get<Line>(primitive);
            Polygon new_line = {line.p1, line.p2};
            bool should_draw = true;
            should_draw = clip_sutherland_hodgman(new_window, new_line);
            if (should_draw) {
                *it = Line{new_line[0], new_line[1]};
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        } else if (std::holds_alternative<Rectangle>(primitive)) {
            auto &rectangle = std::get<Rectangle>(primitive);
            Polygon new_polygon = cast_rectangle_to_polygon(rectangle);
            bool should_draw = true;
            should_draw = clip_sutherland_hodgman(new_window, new_polygon);
            if (should_draw) {
                *it = new_polygon;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        } else if (std::holds_alternative<Polygon>(primitive)) {
            auto &polygon = std::get<Polygon>(primitive);
            Polygon new_polygon = polygon;
            bool should_draw = true;
            should_draw = clip_sutherland_hodgman(new_window, new_polygon);
            if (should_draw) {
                *it = new_polygon;
            } else {
                it = render_primitives_.erase(it);
                continue;
            }
        }
        ++it;
    }
}
