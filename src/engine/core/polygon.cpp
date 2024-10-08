//
// Created by Autumn Sound on 2024/9/20.
//
#include <algorithm>

#include "engine.hpp"

using namespace RenderCore;

void RenderEngine::draw_rectangle(const Rectangle &rectangle) {
    // 共享边界的处理
    // 原则：左闭右开，下闭上开。即矩形左边、下边的像素属于矩形。
    for (int x = rectangle.min_x(); x < rectangle.max_x(); x++) {
        for (int y = rectangle.min_y(); y < rectangle.max_y(); y++) {
            draw_pixel(x, y, pen_options_.fill_color);
        }
    }
    // 画边线
    draw_line(
        make_line({rectangle.min_x(), rectangle.min_y()}, {rectangle.max_x(), rectangle.min_y()}));
    draw_line(
        make_line({rectangle.min_x(), rectangle.min_y()}, {rectangle.min_x(), rectangle.max_y()}));
    draw_line(
        make_line({rectangle.max_x(), rectangle.min_y()}, {rectangle.max_x(), rectangle.max_y()}));
    draw_line(
        make_line({rectangle.min_x(), rectangle.max_y()}, {rectangle.max_x(), rectangle.max_y()}));
}

void RenderEngine::draw_polygon(const Polygon &polygon) {
    draw_polygon_scanline(polygon);
    for (size_t i = 0; i < polygon.size(); i++) {
        draw_line(make_line(polygon[i], polygon[(i + 1) % polygon.size()]));
    }
}

void RenderEngine::draw_polygon_scanline(const Polygon &polygon) {
    struct Edge {
        int y_max;
        float x, dx;
        Edge *next;
    };

    // 1. 初始化边表
    std::vector<Edge *> edge_table(height_);
    for (int i = 0; i < height_; i++) {
        edge_table[i] = nullptr;
    }
    // 2. 构建边表
    for (size_t i = 0; i < polygon.size(); i++) {
        auto start = polygon[i];
        auto end = polygon[(i + 1) % polygon.size()];
        if (start.y == end.y) {
            continue;
        }
        if (start.y > end.y) {
            std::swap(start, end);
        }
        Edge *edge = new Edge();
        edge->y_max = end.y;
        edge->x = start.x;
        edge->dx = static_cast<float>(end.x - start.x) / (end.y - start.y);
        edge->next = edge_table[start.y];
        edge_table[start.y] = edge;
    }
    // 3. 初始化活性边表
    std::vector<Edge *> active_edge_table;
    // 4. 扫描线填充
    for (int y = 0; y < height_;) {
        // 4.1 更新活性边表
        for (auto edge = edge_table[y]; edge; edge = edge->next) {
            active_edge_table.push_back(edge);
        }
        // 4.2 按 x 排序
        std::sort(active_edge_table.begin(), active_edge_table.end(),
            [](const Edge *lhs, const Edge *rhs) { return lhs->x < rhs->x; });
        // 4.3 填充
        for (size_t i = 0; i < active_edge_table.size(); i += 2) {
            int x1 = std::ceil(active_edge_table[i]->x);
            int x2 = std::floor(active_edge_table[i + 1]->x);
            for (int x = x1; x <= x2; x++) {
                draw_pixel(x, y, pen_options_.fill_color);
            }
        }
        // 4.4 更新 x
        for (auto &edge : active_edge_table) {
            edge->x += edge->dx;
        }
        y++;
        // 4.5 移除不再活动的边
        active_edge_table.erase(std::remove_if(active_edge_table.begin(), active_edge_table.end(),
                                    [y](const Edge *edge) { return edge->y_max == y; }),
            active_edge_table.end());
    }
    // 5. 释放内存
    for (int i = 0; i < height_; i++) {
        auto edge = edge_table[i];
        while (edge) {
            auto next = edge->next;
            delete edge;
            edge = next;
        }
    }
}
