//
// Created by Autumn Sound on 2024/9/23.
//

#ifndef RENDERENGINE_CLIP_HPP
#define RENDERENGINE_CLIP_HPP

#include <variant>

#include "polygon.hpp"
#include "rectangle.hpp"

namespace RenderCore {

struct Clip {
    bool enable{false};
    std::variant<Rectangle, Polygon> window;
};

}  // namespace RenderCore

#endif  //RENDERENGINE_CLIP_HPP
