//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_FILL_HPP
#define RENDERENGINE_FILL_HPP

#include "point.hpp"

namespace RenderCore {

struct Fill {
    Point seed;
};

inline Fill make_fill(const Point &seed) { return Fill{seed}; }

}  // namespace RenderCore

#endif
