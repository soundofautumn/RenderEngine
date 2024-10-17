//
// Created by Autumn Sound on 2024/9/6.
//
#include "circle.hpp"

#include <cmath>
#include <limits>
#include <numbers>
#include <utility>

#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_circle_midpoint(const Point &center, int radius) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;  // 初始决策参数

    // 绘制圆的八个对称点
    auto draw_circle_points = [&](int cx, int cy) {
        draw_point(cx + x, cy + y);
        draw_point(cx - x, cy + y);
        draw_point(cx + x, cy - y);
        draw_point(cx - x, cy - y);
        draw_point(cx + y, cy + x);
        draw_point(cx - y, cy + x);
        draw_point(cx + y, cy - x);
        draw_point(cx - y, cy - x);
    };

    draw_circle_points(center.x, center.y);  // 绘制初始点

    while (x < y) {
        if (d < 0) {
            d += 2 * x + 3;  // 更新决策参数
        } else {
            d += 2 * (x - y) + 5;  // 更新决策参数
            y--;                   // y 减小
        }
        x++;                                     // x 增加
        draw_circle_points(center.x, center.y);  // 绘制对称点
    }
}

void RenderEngine::draw_arc_midpoint(
    const Point &center, int radius, float start_angle, float end_angle) {
    if (start_angle >= end_angle) {
        std::swap(start_angle, end_angle);
    }
    auto draw_arc_points = [&](int x, int y) {
        // 判断点是否在圆弧内
        auto angle = std::atan2(y - center.y, x - center.x);
        if (angle >= start_angle && angle <= end_angle) {
            draw_point(x, y);
        }
    };

    int x = 0;
    int y = radius;
    int d = 1 - radius;  // 初始决策参数

    // 绘制圆的八个对称点
    auto draw_circle_points = [&](int cx, int cy) {
        draw_arc_points(cx + x, cy + y);
        draw_arc_points(cx - x, cy + y);
        draw_arc_points(cx + x, cy - y);
        draw_arc_points(cx - x, cy - y);
        draw_arc_points(cx + y, cy + x);
        draw_arc_points(cx - y, cy + x);
        draw_arc_points(cx + y, cy - x);
        draw_arc_points(cx - y, cy - x);
    };

    draw_circle_points(center.x, center.y);  // 绘制初始点

    while (x < y) {
        if (d < 0) {
            d += 2 * x + 3;  // 更新决策参数
        } else {
            d += 2 * (x - y) + 5;  // 更新决策参数
            y--;                   // y 减小
        }
        x++;                                     // x 增加
        draw_circle_points(center.x, center.y);  // 绘制对称点
    }
}

// 根据三点求圆心和半径
std::pair<Point, int> circle_center_radius(const Point &p1, const Point &p2, const Point &p3) {
    // 两条中垂线的交点即为圆心
    // 两条中垂线分别为：
    // 1. 由 p1, p2 两点的中点和斜率为 -(x2 - x1) / (y2 - y1) 的直线
    // 2. 由 p2, p3 两点的中点和斜率为 -(x3 - x2) / (y3 - y2) 的直线
    // 两直线的交点即为圆心
    auto mid_point1 = (p1 + p2) / 2;
    auto mid_point2 = (p2 + p3) / 2;
    float k1, k2;
    if (p2.y != p1.y) {
        k1 = -(p2.x - p1.x) / static_cast<float>(p2.y - p1.y);
    } else {
        k1 = std::numeric_limits<float>::infinity();
    }
    if (p3.y != p2.y) {
        k2 = -(p3.x - p2.x) / static_cast<float>(p3.y - p2.y);
    } else {
        k2 = std::numeric_limits<float>::infinity();
    }
    float x, y;
    if (k1 == std::numeric_limits<float>::infinity()) {
        x = mid_point1.x;
        y = k2 * (x - mid_point2.x) + mid_point2.y;
    } else if (k2 == std::numeric_limits<float>::infinity()) {
        x = mid_point2.x;
        y = k1 * (x - mid_point1.x) + mid_point1.y;
    } else {
        x = (mid_point2.y - mid_point1.y + k1 * mid_point1.x - k2 * mid_point2.x) / (k1 - k2);
        y = k1 * (x - mid_point1.x) + mid_point1.y;
    }
    auto center = Point{static_cast<int>(x), static_cast<int>(y)};
    auto radius = vector_length(center - p1);
    return {center, radius};
}

void RenderEngine::draw_circle(const Circle &circle) {
    if (std::holds_alternative<CircleUseCenterRadius>(circle)) {
        auto &center_radius = std::get<CircleUseCenterRadius>(circle);
        draw_circle_midpoint(center_radius.center, center_radius.radius);
    } else if (std::holds_alternative<CircleUseThreePoints>(circle)) {
        auto &three_points = std::get<CircleUseThreePoints>(circle);
        // 根据三点求圆心和半径
        auto [center, radius] =
            circle_center_radius(three_points.p1, three_points.p2, three_points.p3);
        draw_circle_midpoint(center, radius);
    }
}

void RenderEngine::draw_arc(const Arc &arc) {
    if (std::holds_alternative<ArcUseCenterRadiusAngle>(arc)) {
        auto &center_radius_angle = std::get<ArcUseCenterRadiusAngle>(arc);
        draw_arc_midpoint(center_radius_angle.center, center_radius_angle.radius,
            center_radius_angle.start_angle, center_radius_angle.end_angle);
    } else if (std::holds_alternative<ArcUseThreePoints>(arc)) {
        auto &three_points = std::get<ArcUseThreePoints>(arc);
        // 根据三点求圆心和半径
        auto [center, radius] =
            circle_center_radius(three_points.p1, three_points.p2, three_points.p3);
        // 计算起始角度和终止角度
        auto start_angle = std::atan2(three_points.p1.y - center.y, three_points.p1.x - center.x);
        auto end_angle = std::atan2(three_points.p3.y - center.y, three_points.p3.x - center.x);
        if (start_angle > end_angle) {
            std::swap(start_angle, end_angle);
        }
        auto p2place = std::atan2(three_points.p2.y - center.y, three_points.p2.x - center.x);
        if (p2place >= start_angle && p2place <= end_angle) {
            draw_arc_midpoint(center, radius, start_angle, end_angle);
        } else {
            draw_arc_midpoint(center, radius, -std::numbers::pi, start_angle);
            draw_arc_midpoint(center, radius, end_angle, std::numbers::pi);
        }
    }
}
