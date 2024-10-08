//
// Created by Autumn Sound on 2024/9/30.
//
#include "transform.hpp"

#include <cmath>
#include <variant>

#include "engine.hpp"
#include "matrix.hpp"

using namespace RenderCore;

constexpr Matrix3f make_translate_matrix(float x, float y) {
    // 1  0  x
    // 0  1  y
    // 0  0  1
    return {{{1, 0, x}, {0, 1, y}, {0, 0, 1}}};
}

constexpr Matrix3f make_rotate_matrix(float radian) {
    const auto c = static_cast<float>(std::cos(radian));
    const auto s = static_cast<float>(std::sin(radian));
    // c -s  0
    // s  c  0
    // 0  0  1
    return {{{c, -s, 0}, {s, c, 0}, {0, 0, 1}}};
}

constexpr Matrix3f make_scale_matrix(float x, float y) {
    // x  0  0
    // 0  y  0
    // 0  0  1
    return {{{x, 0, 0}, {0, y, 0}, {0, 0, 1}}};
}

constexpr Matrix3f make_transform_matrix(const Transform &transform) {
    if (std::holds_alternative<Translate>(transform)) {
        const auto &translate = std::get<Translate>(transform);
        return make_translate_matrix(translate.offset.x, translate.offset.y);
    } else if (std::holds_alternative<Rotate>(transform)) {
        const auto &rotate = std::get<Rotate>(transform);
        return make_translate_matrix(rotate.center.x, rotate.center.y) *
               make_rotate_matrix(rotate.angle) *
               make_translate_matrix(-rotate.center.x, -rotate.center.y);
    } else if (std::holds_alternative<Scale>(transform)) {
        const auto &scale = std::get<Scale>(transform);
        return make_translate_matrix(scale.center.x, scale.center.y) *
               make_scale_matrix(scale.scale.x, scale.scale.y) *
               make_translate_matrix(-scale.center.x, -scale.center.y);
    }
    return Matrix3f::identity();
}

void RenderEngine::make_transform(const Transform &transform) {
    transform_matrix_ = make_transform_matrix(transform) * transform_matrix_;
}
