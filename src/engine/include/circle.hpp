//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_CIRCLE_HPP
#define RENDERENGINE_CIRCLE_HPP

#include "options.hpp"
#include "point.hpp"

struct Circle {
    // 确定圆的方式：中心点和半径/三个点
    enum class Type {
        CenterRadius,
        ThreePoints
    } type{Type::CenterRadius};

    struct {
        Point center;
        int radius;
    } center_radius;

    struct {
        Point p1, p2, p3;
    } three_points;

    PenOptions options;
};

inline Circle make_circle_center_radius(const Point &center, int radius, const PenOptions &options) {
    return Circle{.type=Circle::Type::CenterRadius, .center_radius = {center, radius}, .options = options};
}

struct Arc {
    // 确定圆弧的方式：中心点、半径、起始角度和终止角度
    enum class Type {
        CenterRadiusAngle,
        ThreePoints
    } type{Type::CenterRadiusAngle};

    struct {
        Point center;
        int radius;
        float start_angle;
        float end_angle;
    } center_radius_angle;

    struct {
        Point p1, p2, p3;
    } three_points;

    PenOptions options;
};

inline Arc make_arc_center_radius_angle(const Point &center, int radius, float start_angle, float end_angle, const PenOptions &options) {
    return Arc{.type=Arc::Type::CenterRadiusAngle, .center_radius_angle = {center, radius, start_angle, end_angle}, .options = options};
}

#endif //RENDERENGINE_CIRCLE_HPP
