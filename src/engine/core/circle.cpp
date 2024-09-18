//
// Created by Autumn Sound on 2024/9/6.
//

#include "circle.hpp"

#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_circle_midpoint(
    const Point &center, int radius, const PenOptions &options) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;  // 初始决策参数

    // 绘制圆的八个对称点
    auto draw_circle_points = [&](int cx, int cy) {
        draw_point(cx + x, cy + y, options);
        draw_point(cx - x, cy + y, options);
        draw_point(cx + x, cy - y, options);
        draw_point(cx - x, cy - y, options);
        draw_point(cx + y, cy + x, options);
        draw_point(cx - y, cy + x, options);
        draw_point(cx + y, cy - x, options);
        draw_point(cx - y, cy - x, options);
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

void RenderEngine::draw_arc_midpoint(const Point &center, int radius, float start_angle,
    float end_angle, const PenOptions &options) {
    // 将角度转换为弧度
    auto to_radians = [](float degrees) { return degrees * (3.1415926 / 180.0f); };
    float start_radian = to_radians(start_angle);
    float end_radian = to_radians(end_angle);

    // 计算弧的角度范围
    float angle_range = end_radian - start_radian;
    if (angle_range < 0) angle_range += 2 * 3.1415926;  // 处理负角度范围的情况

    int x = radius;
    int y = 0;
    int d = 1 - radius;

    // 绘制起始和结束角度之间的弧
    auto draw_arc_points = [&](int cx, int cy, int x, int y) {
        // 计算弧度范围的步长，控制每次角度增加的精度
        float step = 0.001;  // 可以根据需要调整步长的大小，步长越小，圆弧越光滑

        if (start_radian <= end_radian) {  // 起点角度值不大于终点角度值
            // 从起始角度绘制到结束角度
            for (float theta = start_radian; theta <= end_radian; theta += step) {
                int x = cx + radius * cos(theta);  // 计算极坐标的 x 坐标
                int y = cy + radius * sin(theta);  // 计算极坐标的 y 坐标

                // 绘制当前角度下的点
                draw_point(x, 2 * cy - y, options);
            }
        } else {  // 起点角度值大于终点角度值
            for (float theta = end_radian; theta <= start_radian; theta += step) {
                int x = cx + radius * cos(theta);  // 计算极坐标的 x 坐标
                int y = cy + radius * sin(theta);  // 计算极坐标的 y 坐标

                // 绘制当前角度下的点
                draw_point(x, 2 * cy - y, options);
            }
        }
    };

    // 绘制圆弧
    draw_arc_points(center.x, center.y, x, y);
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
    auto k1 = -(p2.x - p1.x) / (p2.y - p1.y);
    auto k2 = -(p3.x - p2.x) / (p3.y - p2.y);
    auto x = (mid_point2.y - mid_point1.y + k1 * mid_point1.x - k2 * mid_point2.x) / (k1 - k2);
    auto y = k1 * (x - mid_point1.x) + mid_point1.y;
    auto center = Point{static_cast<int>(x), static_cast<int>(y)};
    auto radius = vector_length(center - p1);
    return {center, radius};
}

void RenderEngine::draw_circle(const Circle &circle) {
    if (std::holds_alternative<CircleUseCenterRadius>(circle)) {
        auto &center_radius = std::get<CircleUseCenterRadius>(circle);
        draw_circle_midpoint(center_radius.center, center_radius.radius, center_radius.options);
        return;
    } else if (std::holds_alternative<CircleUseThreePoints>(circle)) {
        auto &three_points = std::get<CircleUseThreePoints>(circle);
        // 根据三点求圆心和半径
        auto [center, radius] =
            circle_center_radius(three_points.p1, three_points.p2, three_points.p3);
        draw_circle_midpoint(center, radius, three_points.options);
    }
}

void RenderEngine::draw_arc(const Arc &arc) {
    if (std::holds_alternative<ArcUseCenterRadiusAngle>(arc)) {
        auto &center_radius_angle = std::get<ArcUseCenterRadiusAngle>(arc);
        draw_arc_midpoint(center_radius_angle.center, center_radius_angle.radius,
            center_radius_angle.start_angle, center_radius_angle.end_angle,
            center_radius_angle.options);
        return;
    } else if (std::holds_alternative<ArcUseThreePoints>(arc)) {
        auto &three_points = std::get<ArcUseThreePoints>(arc);
        // 根据三点求圆心和半径
        auto [center, radius] =
            circle_center_radius(three_points.p1, three_points.p2, three_points.p3);
        // 计算起始角度和终止角度
        auto start_angle = atan2(three_points.p1.y - center.y, three_points.p1.x - center.x);
        auto end_angle = atan2(three_points.p3.y - center.y, three_points.p3.x - center.x);
        draw_arc_midpoint(center, radius, start_angle, end_angle, three_points.options);
    }
}
