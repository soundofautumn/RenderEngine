//
// Created by Autumn Sound on 2024/9/30.
//

#ifndef RENDERENGINE_TRANSFORM_HPP
#define RENDERENGINE_TRANSFORM_HPP

#include <variant>

#include "point.hpp"

namespace RenderCore {

// 平移变换
struct Translate {
    Point offset;
};

// 旋转变换
struct Rotate {
    float angle;
    Point center;
};

// 缩放变换
struct Scale {
    float scale_x;
    float scale_y;
    Point center;
};

// 变换
using Transform = std::variant<Translate, Rotate, Scale>;

}  // namespace RenderCore

#endif  //RENDERENGINE_TRANSFORM_HPP
