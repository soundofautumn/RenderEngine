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
    enum class Action { Draw, Clip };
    Action action{Action::Draw};
};

inline Polygon make_polygon(
    const std::vector<Point> &points, Polygon::Action action = Polygon::Action::Draw) {
    return Polygon{points, .action = action};
};

}  // namespace RenderCore
#endif
