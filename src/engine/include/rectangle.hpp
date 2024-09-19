//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_RECTANGLE_HPP
#define RENDERENGINE_RECTANGLE_HPP

#include "point.hpp"
#include "vector.hpp"

namespace RenderCore {

struct Rectangle {
    Vector<4, Point> points;
};

}  // namespace RenderCore
#endif  //RENDERENGINE_RECTANGLE_HPP
