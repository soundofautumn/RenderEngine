//
// Created by Autumn Sound on 2024/9/11.
//

#include "engine.hpp"
#include "options.hpp"

using namespace RenderCore;

// 虚线
const uint16_t DASH_PATTERN = 0b1100'1100'1100'1100;
// 点线
const uint16_t DOT_PATTERN = 0b1000'1000'1000'1000;
// 点划线
const uint16_t DASH_DOT_PATTERN = 0b1111'1010'1111'1010;

void RenderEngine::draw_point(int x, int y, const PenOptions &options, int index) {
    if (options.type == PenOptions::LineType::SOLID && options.width <= 1) {
        draw_pixel(x, y, options.color);
        return;
    }
    if (options.type != PenOptions::LineType::SOLID && index == -1) {
        throw std::runtime_error("Unsupported line type");
    }
    const auto draw = [&](int x, int y) {
        // 以这个点为中心画一个正方形
        for (int i = -options.width / 2; i <= options.width / 2; i++) {
            for (int j = -options.width / 2; j <= options.width / 2; j++) {
                draw_pixel(x + i, y + j, options.color);
            }
        }
    };
    index = (index / options.dash) % 16;
    switch (options.type) {
        case PenOptions::LineType::SOLID:
            draw(x, y);
            break;
        case PenOptions::LineType::DASH:
            if ((DASH_PATTERN >> index) & 1) {
                draw(x, y);
            }
            break;
        case PenOptions::LineType::DOT:
            if ((DOT_PATTERN >> index) & 1) {
                draw(x, y);
            }
            break;
        case PenOptions::LineType::DASH_DOT:
            if ((DASH_DOT_PATTERN >> index) & 1) {
                draw(x, y);
            }
            break;
    }
}