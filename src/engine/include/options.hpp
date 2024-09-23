//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_OPTIONS_HPP
#define RENDERENGINE_OPTIONS_HPP

#include "clip.hpp"
#include "color.hpp"
#include "point.hpp"

namespace RenderCore {

// 画笔选项
struct PenOptions {
    // 画笔颜色
    Color color{Colors::White};
    // 填充颜色
    Color fill_color{Colors::Black};
    // 画笔宽度
    int width{1};
    // 画笔类型
    enum class LineType {
        SOLID,     // 实线
        DASH,      // 虚线
        DOT,       // 点线
        DASH_DOT,  // 点划线
    } type{LineType::SOLID};
    // 虚线间隔
    int dash{5};
};

// 全局选项
struct GlobalOptions {
    // 背景色
    Color background_color{Colors::Black};
    // 裁剪窗口
    Clip clip;
};

}  // namespace RenderCore

#endif  //RENDERENGINE_OPTIONS_HPP
