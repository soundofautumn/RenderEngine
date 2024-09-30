//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_COLOR_HPP
#define RENDERENGINE_COLOR_HPP

#include "vector.hpp"

namespace RenderCore {

using Color = Vector4f;

// 矢量转整数颜色
constexpr inline static uint32_t vector_to_color(const Vector4f &v) {
    // 截断到[0, 1]范围
    const auto &color = vector_clamp(v, 0.0f, 1.0f) * 255.0f;
    const auto r = static_cast<uint32_t>(color.r);
    const auto g = static_cast<uint32_t>(color.g);
    const auto b = static_cast<uint32_t>(color.b);
    const auto a = static_cast<uint32_t>(color.a);
    return (a << 24) | (b << 16) | (g << 8) | r;
}

constexpr inline static uint32_t vector_to_color(const Vector3f &v) {
    return vector_to_color(v.xyz1());
}

// 整数颜色转矢量
constexpr inline static Vector4f color_to_vector(uint32_t color) {
    const auto r = static_cast<float>(color & 0xFF);
    const auto g = static_cast<float>((color >> 8) & 0xFF);
    const auto b = static_cast<float>((color >> 16) & 0xFF);
    const auto a = static_cast<float>((color >> 24) & 0xFF);
    return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}

namespace Colors {
const Color Black{0, 0, 0, 1};
const Color White{1, 1, 1, 1};
const Color Red{1, 0, 0, 1};
const Color Green{0, 1, 0, 1};
const Color Blue{0, 0, 1, 1};
const Color Yellow{1, 1, 0, 1};
const Color Cyan{0, 1, 1, 1};
const Color Magenta{1, 0, 1, 1};
const Color Gray{0.5, 0.5, 0.5, 1};
const Color LightGray{0.75, 0.75, 0.75, 1};
const Color DarkGray{0.25, 0.25, 0.25, 1};
}  // namespace Colors

}  // namespace RenderCore

#endif  //RENDERENGINE_COLOR_HPP
