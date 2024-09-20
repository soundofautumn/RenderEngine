//
// Created by Autumn Sound on 2024/9/19.
//

#ifndef RENDERENGINE_RECTANGLE_HPP
#define RENDERENGINE_RECTANGLE_HPP

#include "point.hpp"
#include "utils.hpp"
#include "vector.hpp"

namespace RenderCore {

struct Rectangle {
    union {
        struct {
            Point top_left;
            Point bottom_right;
        };
        struct {
            int min_x;
            int min_y;
            int max_x;
            int max_y;
        };
    };
};

}  // namespace RenderCore
#endif  //RENDERENGINE_RECTANGLE_HPP
