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

void RenderEngine::draw_bspline_curve(const BsplineCurve &curve) {
    const auto &control_points = curve.control_points;
    const auto &knots = curve.knots;
    const auto n = control_points.size();
    const auto p = knots.size() - n - 1;
    std::vector<std::vector<Vector2f>> points(p + 1);
    for (size_t i = 0; i < p + 1; i++) {
        points[i].resize(n);
        for (size_t j = 0; j < n; j++) {
            points[i][j] = Vector2f{
                static_cast<float>(control_points[j].x), static_cast<float>(control_points[j].y)};
        }
    }

    std::function<Vector2f(const std::vector<Vector2f> &, float, int, int)> de_boor =
        [&](const std::vector<Vector2f> &points, float t, int i, int k) -> Vector2f {
        if (k == 0) {
            return points[i];
        }
        return (t - knots[i]) / (knots[i + k] - knots[i]) * de_boor(points, t, i, k - 1) +
               (knots[i + k + 1] - t) / (knots[i + k + 1] - knots[i + 1]) *
                   de_boor(points, t, i + 1, k - 1);
    };

    for (float t = knots[p]; t <= knots[n]; t += 0.001) {
        auto point = de_boor(points[p], t, 0, p);
        draw_point(point.x, point.y);
    }
}
