//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_RECTANGLE_HPP
#define RENDERENGINE_RECTANGLE_HPP

#include "point.hpp"
#include "polygon.hpp"
#include "vector.hpp"

namespace RenderCore {

struct Rectangle {
    Point top_left;
    Point bottom_right;

    int &min_x() { return top_left.x; }
    int &min_y() { return top_left.y; }
    int &max_x() { return bottom_right.x; }
    int &max_y() { return bottom_right.y; }

    int min_x() const { return top_left.x; }
    int min_y() const { return top_left.y; }
    int max_x() const { return bottom_right.x; }
    int max_y() const { return bottom_right.y; }
};

inline Rectangle make_rectangle(const Point &top_left, const Point &bottom_right) {
    Rectangle rectangle;
    rectangle.top_left = top_left;
    rectangle.bottom_right = bottom_right;
    return rectangle;
}

// 多边形转矩形
inline bool try_cast_polygon_to_rectangle(const Polygon &polygon, Rectangle &rectangle) {
    if (polygon.size() != 4) {
        return false;
    }
    // 找出x坐标和y坐标的最小值和最大值
    int min_x = std::min({polygon[0].x, polygon[1].x, polygon[2].x, polygon[3].x});
    int max_x = std::max({polygon[0].x, polygon[1].x, polygon[2].x, polygon[3].x});
    int min_y = std::min({polygon[0].y, polygon[1].y, polygon[2].y, polygon[3].y});
    int max_y = std::max({polygon[0].y, polygon[1].y, polygon[2].y, polygon[3].y});

    // 验证这四个点是否构成平行于坐标轴的矩形
    int x_count = 0;
    int y_count = 0;
    for (const auto &point : polygon) {
        if (point.x == min_x || point.x == max_x) {
            ++x_count;
        }
        if (point.y == min_y || point.y == max_y) {
            ++y_count;
        }
    }

    // x_count和y_count都应该等于4，说明点分布在两条平行于x轴和y轴的线上
    if (x_count == 4 && y_count == 4) {
        // 更新矩形的坐标
        rectangle.min_x() = min_x;
        rectangle.min_y() = min_y;
        rectangle.max_x() = max_x;
        rectangle.max_y() = max_y;
        return true;
    }

    return true;
}

// 矩形转多边形
inline Polygon cast_rectangle_to_polygon(const Rectangle &rectangle) {
    Polygon polygon;
    polygon.emplace_back(rectangle.min_x(), rectangle.min_y());
    polygon.emplace_back(rectangle.max_x(), rectangle.min_y());
    polygon.emplace_back(rectangle.max_x(), rectangle.max_y());
    polygon.emplace_back(rectangle.min_x(), rectangle.max_y());
    return polygon;
}

}  // namespace RenderCore
#endif  //RENDERENGINE_RECTANGLE_HPP
