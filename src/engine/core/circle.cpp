//
// Created by Autumn Sound on 2024/9/6.
//

#include "circle.hpp"
#include "engine.hpp"

void draw_circle_by_midpoint(RenderEngine *engine, const Point &center, int radius, const PenOptions &options) {
    int x = 0;
    int y = radius;
    int d = 1 - radius; // 初始决策参数

    // 绘制圆的八个对称点
    auto draw_circle_points = [&](int cx, int cy) {
        engine->draw_point(cx + x, cy + y, options);
        engine->draw_point(cx - x, cy + y, options);
        engine->draw_point(cx + x, cy - y, options);
        engine->draw_point(cx - x, cy - y, options);
        engine->draw_point(cx + y, cy + x, options);
        engine->draw_point(cx - y, cy + x, options);
        engine->draw_point(cx + y, cy - x, options);
        engine->draw_point(cx - y, cy - x, options);
    };

    draw_circle_points(center.x, center.y); // 绘制初始点

    while (x < y) {
        if (d < 0) {
            d += 2 * x + 3; // 更新决策参数
        } else {
            d += 2 * (x - y) + 5; // 更新决策参数
            y--; // y 减小
        }
        x++; // x 增加
        draw_circle_points(center.x, center.y); // 绘制对称点
    }
}

void draw_arc_by_midpoint(RenderEngine *engine,
                          const Point &center, int radius,
                          float start_angle, float end_angle,
                          const PenOptions &options) {
    throw std::runtime_error("Not implemented yet");
}

void RenderEngine::draw_circle(const Circle &circle) {
    if (std::holds_alternative<CircleUseCenterRadius>(circle)) {
        auto &center_radius = std::get<CircleUseCenterRadius>(circle);
        draw_circle_by_midpoint(this, center_radius.center, center_radius.radius, center_radius.options);
        return;
    } else if (std::holds_alternative<CircleUseThreePoints>(circle)) {
        auto &three_points = std::get<CircleUseThreePoints>(circle);
        throw std::runtime_error("Not implemented yet");
    }
}

void RenderEngine::draw_arc(const Arc &arc) {
    if (std::holds_alternative<ArcUseCenterRadiusAngle>(arc)) {
        auto &center_radius_angle = std::get<ArcUseCenterRadiusAngle>(arc);
        draw_arc_by_midpoint(this, center_radius_angle.center, center_radius_angle.radius,
                             center_radius_angle.start_angle, center_radius_angle.end_angle,
                             center_radius_angle.options);
        return;
    } else if (std::holds_alternative<ArcUseThreePoints>(arc)) {
        auto &three_points = std::get<ArcUseThreePoints>(arc);
        throw std::runtime_error("Not implemented yet");
    }
}