//
// Created by Autumn Sound on 2024/9/30.
//
#include "transform.hpp"

#include <variant>

#include "engine.hpp"
#include "matrix.hpp"

using namespace RenderCore;

constexpr Matrix3f make_translate(float x, float y) {
    // 1  0  x
    // 0  1  y
    // 0  0  1
    return {{{1, 0, x}, {0, 1, y}, {0, 0, 1}}};
}

constexpr Matrix3f make_rotate(float radian) {
    const auto c = cos(radian);
    const auto s = sin(radian);
    // c -s  0
    // s  c  0
    // 0  0  1
    return {{{c, -s, 0}, {s, c, 0}, {0, 0, 1}}};
}

constexpr Matrix3f make_scale(float x, float y) {
    // x  0  0
    // 0  y  0
    // 0  0  1
    return {{{x, 0, 0}, {0, y, 0}, {0, 0, 1}}};
}

void RenderEngine::make_transform(const Transform &transform) {
    if (std::holds_alternative<Translate>(transform)) {
        const auto &translate = std::get<Translate>(transform);
        const auto offset = translate.offset;
        transform_matrix_ = make_translate(offset.x, offset.y) * transform_matrix_;
    } else if (std::holds_alternative<Rotate>(transform)) {
        const auto &rotate = std::get<Rotate>(transform);
        const auto angle = rotate.angle;
        const auto center = rotate.center;
        transform_matrix_ = make_translate(center.x, center.y) * make_rotate(angle) *
                            make_translate(-center.x, -center.y) * transform_matrix_;
    } else if (std::holds_alternative<Scale>(transform)) {
        const auto &scale = std::get<Scale>(transform);
        const auto scale_x = scale.scale_x;
        const auto scale_y = scale.scale_y;
        const auto center = scale.center;
        transform_matrix_ = make_translate(center.x, center.y) * make_scale(scale_x, scale_y) *
                            make_translate(-center.x, -center.y) * transform_matrix_;
    }
}
