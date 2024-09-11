//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_CIRCLE_HPP
#define RENDERENGINE_CIRCLE_HPP

#include "options.hpp"
#include "point.hpp"
#include <variant>

struct CircleUseCenterRadius {
    Point center;
    int radius;
    PenOptions options;
};

struct CircleUseThreePoints {
    Point p1, p2, p3;
    PenOptions options;
};

using Circle = std::variant<CircleUseCenterRadius, CircleUseThreePoints>;

inline Circle make_circle_center_radius(const Point &center, int radius, const PenOptions &options) {
    return CircleUseCenterRadius{center, radius, options};
}

inline Circle make_circle_three_points(const Point &p1, const Point &p2, const Point &p3, const PenOptions &options) {
    return CircleUseThreePoints{p1, p2, p3, options};
}

struct ArcUseCenterRadiusAngle {
    Point center;
    int radius;
    float start_angle;
    float end_angle;
    PenOptions options;
};

struct ArcUseThreePoints {
    Point p1, p2, p3;
    PenOptions options;
};

using Arc = std::variant<ArcUseCenterRadiusAngle, ArcUseThreePoints>;

inline Arc make_arc_center_radius_angle(const Point &center, int radius, float start_angle, float end_angle,
                                        const PenOptions &options) {
    return ArcUseCenterRadiusAngle{center, radius, start_angle, end_angle, options};
}

#endif //RENDERENGINE_CIRCLE_HPP
