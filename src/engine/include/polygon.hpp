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
    enum class Action { Draw, Fill, Clip };
    Action action{Action::Draw};
};

}  // namespace RenderCore
#endif
