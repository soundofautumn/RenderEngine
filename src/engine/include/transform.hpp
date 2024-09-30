//
// Created by Autumn Sound on 2024/9/30.
//

#ifndef RENDERENGINE_TRANSFORM_HPP
#define RENDERENGINE_TRANSFORM_HPP

#include <variant>

#include "point.hpp"
#include "vector.hpp"

namespace RenderCore {

// 平移变换
struct Translate {
    Vector2f offset;
};

inline Translate make_translate(float x, float y) { return {{x, y}}; }

// 旋转变换
struct Rotate {
    float angle;
    Point center;
};

inline Rotate make_rotate(float angle, const Point &center) { return {angle, center}; }

// 缩放变换
struct Scale {
    float scale_x;
    float scale_y;
    Point center;
};

inline Scale make_scale(float x, float y, const Point &center) { return {x, y, center}; }

// 变换
using Transform = std::variant<Translate, Rotate, Scale>;

}  // namespace RenderCore

#endif  //RENDERENGINE_TRANSFORM_HPP
