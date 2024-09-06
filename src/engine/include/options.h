//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_OPTIONS_H
#define RENDERENGINE_OPTIONS_H

#include "vector.h"

using Color = Vector4f;

struct PenOptions {
    // 画笔颜色
    Color color{1.0f, 1.0f, 1.0f, 1.0f};
    // 画笔宽度
    float width{1.0f};
    // 画笔类型
    enum class LineType {
        SOLID,
        DASH,
        DOT,
        DASH_DOT,
        DASH_DOT_DOT
    } type{LineType::SOLID};
    // 虚线间隔
    float dash_gap{5.0f};
    // 端点类型
    enum class CapType {
        // 平头
        BUTT,
        // 圆头
        ROUND,
        // 方头
        SQUARE
    } cap{CapType::BUTT};
};

enum class LineAlgorithm {
    DDA,
    MIDPOINT,
    BRESENHAM
};

#endif //RENDERENGINE_OPTIONS_H
