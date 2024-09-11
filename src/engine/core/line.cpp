//
// Created by Autumn Sound on 2024/9/6.
//

#include <cmath>
#include "line.hpp"
#include "utils.hpp"
#include "engine.hpp"

using namespace RenderCore;

// 画特殊斜率的线段
bool draw_line_special_slope(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    int index = 0;
    if (p1 == p2) {
        engine->draw_point(p1.x, p1.y, options, 0);
        return true;
    } else if (p1.x == p2.x) {
        int y1 = min(p1.y, p2.y);
        int y2 = max(p1.y, p2.y);
        int x = p1.x;
        for (int y = y1; y <= y2; ++y) {
            engine->draw_point(x, y, options, index++);
        }
        return true;
    } else if (p1.y == p2.y) {
        int x1 = min(p1.x, p2.x);
        int x2 = max(p1.x, p2.x);
        int y = p1.y;
        for (int x = x1; x <= x2; ++x) {
            engine->draw_point(x, y, options, index++);
        }
        return true;
    } else {
        return false;
    }
}

void draw_line_by_dda(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    if (draw_line_special_slope(engine, p1, p2, options)) {
        return;
    }
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;
    int index = 0;

    float k = (float) (y2 - y1) / (float) (x2 - x1);
    if (abs(k) <= 1) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        auto y = (float) y1;
        for (int x = x1; x <= x2; ++x) {
            engine->draw_point(x, (int) round(y), options, index++);
            y += k;
        }
    } else {
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        k = 1 / k;
        auto x = (float) x1;
        for (int y = y1; y <= y2; ++y) {
            engine->draw_point((int) round(x), y, options, index++);
            x += k;
        }
    }
}

void draw_line_by_midpoint(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    if (draw_line_special_slope(engine, p1, p2, options)) {
        return;
    }
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = x2 - x1;
    int dy = y2 - y1;

    int sx = dx > 0 ? 1 : -1;
    int sy = dy > 0 ? 1 : -1;

    dx = abs(dx);
    dy = abs(dy);

    int index = 0;

    // 选择主要的增量方向
    if (dx > dy) {
        // 主要沿 x 方向
        int d = 2 * dy - dx; // 初始决策参数
        for (int x = x1, y = y1; x != x2; x += sx) {
            engine->draw_point(x, y, options, index++); // 绘制当前像素
            if (d > 0) {
                y += sy; // 更新 y 坐标
                d -= 2 * dx; // 更新决策参数
            }
            d += 2 * dy; // 更新决策参数
        }
    } else {
        // 主要沿 y 方向
        int d = 2 * dx - dy; // 初始决策参数
        for (int y = y1, x = x1; y != y2; y += sy) {
            engine->draw_point(x, y, options, index++); // 绘制当前像素
            if (d > 0) {
                x += sx; // 更新 x 坐标
                d -= 2 * dy; // 更新决策参数
            }
            d += 2 * dx; // 更新决策参数
        }
    }

    // 绘制终点
    engine->draw_point(x1, y1, options);

}

void draw_line_by_bresenham(RenderEngine *engine, const Point &p1, const Point &p2, const PenOptions &options) {
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int index = 0;

    while (true) {
        engine->draw_point(x1, y1, options, index++);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void RenderEngine::draw_line(const Line &line) {
    switch (line.algorithm) {
        case Line::LineAlgorithm::DDA:
            draw_line_by_dda(this, line.p1, line.p2, line.options);
            break;
        case Line::LineAlgorithm::MIDPOINT:
            draw_line_by_midpoint(this, line.p1, line.p2, line.options);
            break;
        case Line::LineAlgorithm::BRESENHAM:
            draw_line_by_bresenham(this, line.p1, line.p2, line.options);
            break;
    }
}