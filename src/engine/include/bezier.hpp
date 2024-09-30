//
// Created by Autumn Sound on 2024/9/30.
//
#ifndef RENDERENGINE_BEZIER_HPP
#define RENDERENGINE_BEZIER_HPP

#include <vector>

#include "point.hpp"

namespace RenderCore {

struct BezierCurve : public std::vector<Point> {

    BezierCurve() = default;

    BezierCurve(const std::vector<Point> &control_points) : std::vector<Point>(control_points) {}

    BezierCurve(std::vector<Point> &&control_points)
        : std::vector<Point>(std::move(control_points)) {}

    BezierCurve(const BezierCurve &other) = default;

    BezierCurve(BezierCurve &&other) noexcept = default;

    BezierCurve &operator=(const BezierCurve &other) = default;

    BezierCurve &operator=(BezierCurve &&other) noexcept = default;
};

inline BezierCurve make_bezier_curve(const std::vector<Point> &control_points) {
    return BezierCurve{control_points};
}

}  // namespace RenderCore

#endif
