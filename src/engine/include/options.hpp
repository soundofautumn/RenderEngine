//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_OPTIONS_HPP
#define RENDERENGINE_OPTIONS_HPP

#include "color.hpp"
#include "point.hpp"

// 画笔选项
struct PenOptions {
    // 画笔颜色
    Color color{Colors::White};
    // 画笔宽度
    float width{1.0f};
    // 画笔类型
    enum class LineType {
        SOLID, // 实线
        DASH, // 虚线
        DOT, // 点线
        DASH_DOT, // 点划线
        DASH_DOT_DOT // 点点划线
    } type{LineType::SOLID};
    // 虚线间隔
    float dash_gap{5.0f};
};

enum class LineAlgorithm {
    DDA, // DDA算法
    MIDPOINT, // 中点算法
    BRESENHAM // Bresenham算法
};

#endif //RENDERENGINE_OPTIONS_HPP
