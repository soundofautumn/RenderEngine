//
// Created by Autumn Sound on 2024/9/11.
//

#ifndef RENDERENGINE_PRIMITIVE_HPP
#define RENDERENGINE_PRIMITIVE_HPP

#include <variant>

#include "circle.hpp"
#include "line.hpp"

namespace RenderCore {

using Primitive = std::variant<Line, Circle, Arc>;

}

#endif  //RENDERENGINE_PRIMITIVE_HPP
