//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_POLYGON_HPP
#define RENDERENGINE_POLYGON_HPP

#include "point.hpp"
#include "vector.hpp"

namespace RenderCore {

struct Polygon {
    std::vector<Point> points;

    size_t size() const { return points.size(); }

    Point &operator[](size_t index) { return points[index]; }

    const Point &operator[](size_t index) const { return points[index]; }
};

inline Polygon make_polygon(const std::vector<Point> &points) {
    return Polygon{points};
};

}  // namespace RenderCore
#endif
