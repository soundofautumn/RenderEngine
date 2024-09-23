//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_CIRCLE_HPP
#define RENDERENGINE_CIRCLE_HPP

#include <variant>

#include "point.hpp"

namespace RenderCore {

struct CircleUseCenterRadius {
    Point center;
    int radius;
};

struct CircleUseThreePoints {
    Point p1, p2, p3;
};

using Circle = std::variant<CircleUseCenterRadius, CircleUseThreePoints>;

inline Circle make_circle_center_radius(const Point &center, int radius) {
    return CircleUseCenterRadius{center, radius};
}

inline Circle make_circle_three_points(const Point &p1, const Point &p2, const Point &p3) {
    return CircleUseThreePoints{p1, p2, p3};
}

struct ArcUseCenterRadiusAngle {
    Point center;
    int radius;
    float start_angle;
    float end_angle;
};

struct ArcUseThreePoints {
    Point p1, p2, p3;
};

using Arc = std::variant<ArcUseCenterRadiusAngle, ArcUseThreePoints>;

inline Arc make_arc_center_radius_angle(
    const Point &center, int radius, float start_angle, float end_angle) {
    return ArcUseCenterRadiusAngle{center, radius, start_angle, end_angle};
}

inline Arc make_arc_three_points(const Point &p1, const Point &p2, const Point &p3) {
    return ArcUseThreePoints{p1, p2, p3};
}

}  // namespace RenderCore

#endif  //RENDERENGINE_CIRCLE_HPP
