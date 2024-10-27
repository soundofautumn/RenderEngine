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

// 图元
// 可以是线段、圆、圆弧、矩形、多边形、填充、画笔选项、变换等
// 也可以是空的 monostate
using Primitive = std::variant<Line, Circle, Arc, Rectangle, Polygon, Fill, PenOptions, Transform,
    BezierCurve, BsplineCurve, std::monostate>;

}

#endif  //RENDERENGINE_PRIMITIVE_HPP
