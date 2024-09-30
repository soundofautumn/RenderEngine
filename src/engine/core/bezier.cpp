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
        points.push_back(Vector2f{static_cast<float>(point.x), static_cast<float>(point.y)});
    }
    for (float t = 0; t <= 1; t += 0.001) {
        auto point = de_casteljau(points, t);
        draw_point(point.x, point.y);
    }
}
