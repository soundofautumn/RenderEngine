//
// Created by Autumn Sound on 2024/9/30.
//
#include <functional>
#include <vector>

#include "engine.hpp"
#include "vector.hpp"

using namespace RenderCore;

void RenderEngine::draw_bezier_curve(const BezierCurve &curve) {
    std::function<Vector2f(const std::vector<Vector2f> &, float)> de_casteljau =
        [&](const std::vector<Vector2f> &points, float t) -> Vector2f {
        if (points.size() == 1) {
            return points[0];
        }
        std::vector<Vector2f> new_points;
        for (size_t i = 0; i < points.size() - 1; i++) {
            new_points.push_back((1 - t) * points[i] + t * points[i + 1]);
        }
        return de_casteljau(new_points, t);
    };

    std::vector<Vector2f> points;
    for (const auto &point : curve) {
        points.emplace_back(static_cast<float>(point.x), static_cast<float>(point.y));
    }
    for (float t = 0; t <= 1; t += 0.001) {
        auto point = de_casteljau(points, t);
        draw_point(point.x, point.y);
    }
}

void RenderEngine::draw_bspline_curve(const BsplineCurve &curve) {
    const auto &control_points = curve.control_points;
    const auto &knots = curve.knots;
    // 控制点数量
    auto n = control_points.size();
    // 节点数量
    auto m = knots.size();
    // 阶数
    auto p = m - n - 1;
    assert(m - n - 1 > 0);  // 阶数大于1，p是size_t类型，不能 assert(p > 0)

    std::function<Vector2f(size_t, size_t, float)> de_boor = [&](size_t k, size_t i,
                                                                 float u) -> Vector2f {
        if (k == 0) {
            return {
                static_cast<float>(control_points[i].x), static_cast<float>(control_points[i].y)};
        }

        // 计算alpha系数
        float alpha = (knots[i + p + 1 - k] - knots[i] != 0.0f)
                          ? (u - knots[i]) / (knots[i + p + 1 - k] - knots[i])
                          : 0.0f;

        return (1.0f - alpha) * de_boor(k - 1, i - 1, u) + alpha * de_boor(k - 1, i, u);
    };

    // 寻找对应的节点区间 i 的函数
    auto find_knot_interval = [&](float u) -> size_t {
        for (size_t j = 0; j < knots.size() - 1; ++j) {
            if (u >= knots[j] && u < knots[j + 1]) {
                return j;
            }
        }
        return 0;  // 如果 u 不在范围内，返回 0
    };

    // 绘制曲线
    constexpr int num_samples = 1000;            // 采样点的数量，值越大曲线越平滑
    float u_start = knots[p];                    // 曲线起始参数
    float u_end = knots[n];                      // 曲线结束参数
    float du = (u_end - u_start) / num_samples;  // 每次步进的参数增量

    for (float u = u_start; u <= u_end; u += du) {
        // 寻找对应的节点区间 l
        auto l = find_knot_interval(u);
        // 使用 De Boor 算法计算 u 对应的曲线点
        Vector2f point = de_boor(p, l, u);
        // 绘制点
        draw_point(point.x, point.y);
    }
}
