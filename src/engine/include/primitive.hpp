//
// Created by Autumn Sound on 2024/9/11.
//

#ifndef RENDERENGINE_PRIMITIVE_HPP
#define RENDERENGINE_PRIMITIVE_HPP

#include <variant>

#include "circle.hpp"
#include "line.hpp"
#include "options.hpp"

namespace RenderCore {

using Primitive = std::variant<Line, Circle, Arc, PenOptions>;

}

#endif  //RENDERENGINE_PRIMITIVE_HPP
