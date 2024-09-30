//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_POLYGON_HPP
#define RENDERENGINE_POLYGON_HPP

#include <initializer_list>
#include <vector>

#include "point.hpp"

namespace RenderCore {

struct Polygon : public std::vector<Point> {

    Polygon() = default;

    Polygon(const std::vector<Point>& points) : std::vector<Point>(points) {}

    Polygon(std::vector<Point>&& points) : std::vector<Point>(std::move(points)) {}

    Polygon(std::initializer_list<Point> points) : std::vector<Point>(points) {}

    Polygon(const Polygon& other) = default;

    Polygon(Polygon&& other) noexcept = default;

    Polygon& operator=(const Polygon& other) = default;

    Polygon& operator=(Polygon&& other) noexcept = default;
};

inline Polygon make_polygon(const std::vector<Point>& points) {
    return Polygon{points};
};

}  // namespace RenderCore
#endif
