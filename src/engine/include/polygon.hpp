//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_POLYGON_HPP
#define RENDERENGINE_POLYGON_HPP

#include <vector>

#include "point.hpp"

namespace RenderCore {

using Polygon = std::vector<Point>;

inline Polygon make_polygon(const std::vector<Point> &points) {
    return Polygon{points};
};

}  // namespace RenderCore
#endif
