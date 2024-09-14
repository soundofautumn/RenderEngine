//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_LINE_HPP
#define RENDERENGINE_LINE_HPP

#include "options.hpp"
#include "point.hpp"

namespace RenderCore {

struct Line {
    Point p1, p2;
    PenOptions options;
    enum class LineAlgorithm {
        DDA,       // DDA算法
        MIDPOINT,  // 中点算法
        BRESENHAM  // Bresenham算法
    } algorithm;
};

inline Line make_line(const Point &p1, const Point &p2, const PenOptions &options = {},
    Line::LineAlgorithm algorithm = Line::LineAlgorithm::BRESENHAM) {
    return RenderCore::Line{p1, p2, options};
}

}  // namespace RenderCore

#endif  // RENDERENGINE_LINE_HPP
