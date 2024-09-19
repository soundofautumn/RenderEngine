//
// Created by Autumn Sound on 2024/9/6.
//

#include "line.hpp"

#include <cmath>

#include "engine.hpp"
#include "utils.hpp"

using namespace RenderCore;

void RenderEngine::draw_line_dda(const Point &p1, const Point &p2) {
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    float k = (float)(y2 - y1) / (float)(x2 - x1);
    if (abs(k) <= 1) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        auto y = (float)y1;
        for (int x = x1, index = 0; x <= x2; ++x) {
            draw_point(x, (int)round(y), index++);
            y += k;
        }
    } else {
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        k = 1 / k;
        auto x = (float)x1;
        for (int y = y1, index = 0; y <= y2; ++y) {
            draw_point((int)round(x), y, index++);
            x += k;
        }
    }
}

void RenderEngine::draw_line_midpoint(const Point &p1, const Point &p2) {
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
        int d = 2 * dy - dx;  // 初始决策参数
        for (int x = x1, y = y1; x != x2; x += sx) {
            draw_point(x, y, index++);  // 绘制当前像素
            if (d > 0) {
                y += sy;      // 更新 y 坐标
                d -= 2 * dx;  // 更新决策参数
            }
            d += 2 * dy;  // 更新决策参数
        }
    } else {
        // 主要沿 y 方向
        int d = 2 * dx - dy;  // 初始决策参数
        for (int y = y1, x = x1; y != y2; y += sy) {
            draw_point(x, y, index++);  // 绘制当前像素
            if (d > 0) {
                x += sx;      // 更新 x 坐标
                d -= 2 * dy;  // 更新决策参数
            }
            d += 2 * dx;  // 更新决策参数
        }
    }

    // 绘制终点
    draw_point(x1, y1, index);
}

void RenderEngine::draw_line_bresenham(const Point &p1, const Point &p2) {
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int index = 0;

    while (true) {
        draw_point(x1, y1, index++);

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
    // 画特殊斜率的线段
    if (line.p1 == line.p2) {
        draw_point(line.p1.x, line.p1.y, 0);
        return;
    } else if (line.p1.x == line.p2.x) {
        int y1 = min(line.p1.y, line.p2.y);
        int y2 = max(line.p1.y, line.p2.y);
        int x = line.p1.x;
        for (int y = y1, index = 0; y <= y2; ++y) {
            draw_point(x, y, index++);
        }
        return;
    } else if (line.p1.y == line.p2.y) {
        int x1 = min(line.p1.x, line.p2.x);
        int x2 = max(line.p1.x, line.p2.x);
        int y = line.p1.y;
        for (int x = x1, index = 0; x <= x2; ++x) {
            draw_point(x, y, index++);
        }
        return;
    }
    switch (line.algorithm) {
        case Line::LineAlgorithm::DDA:
            draw_line_dda(line.p1, line.p2);
            break;
        case Line::LineAlgorithm::MIDPOINT:
            draw_line_midpoint(line.p1, line.p2);
            break;
        case Line::LineAlgorithm::BRESENHAM:
            draw_line_bresenham(line.p1, line.p2);
            break;
    }
}
