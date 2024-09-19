//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_POLYGON_HPP
#define RENDERENGINE_POLYGON_HPP

#include <memory>

#include "point.hpp"
#include "vector.hpp"

namespace RenderCore {

struct Polygon {
    int32_t count;
    std::unique_ptr<Point[]> points;
};

}  // namespace RenderCore
#endif
