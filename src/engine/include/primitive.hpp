//
// Created by Autumn Sound on 2024/9/11.
//

#ifndef RENDERENGINE_PRIMITIVE_HPP
#define RENDERENGINE_PRIMITIVE_HPP

#include <variant>

#include "bezier.hpp"
#include "circle.hpp"
#include "fill.hpp"
#include "line.hpp"
#include "options.hpp"
#include "polygon.hpp"
#include "rectangle.hpp"
#include "transform.hpp"
namespace RenderCore {

using Primitive =
    std::variant<Line, Circle, Arc, Rectangle, Polygon, Fill, PenOptions, Transform, BezierCurve>;

}

#endif  //RENDERENGINE_PRIMITIVE_HPP
