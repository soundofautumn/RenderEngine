//
// Created by Autumn Sound on 2024/9/5.
//
#include <cassert>
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <numbers>
#include <ostream>
#include <random>
#include <vector>

#include "color.hpp"
#include "engine.hpp"
#include "line.hpp"
#include "matrix.hpp"
#include "point.hpp"
#include "polygon.hpp"
#include "rectangle.hpp"
#include "transform.hpp"
#include "vector.hpp"

using namespace RenderCore;

static RenderEngine engine;

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

void lab_1();
void lab_2();
void lab_3();
void lab_4();

void ex_3();
void ex_4();

void perf_test();

void render_and_save(const std::string &filename) {
    // 计时
    auto start = std::chrono::high_resolution_clock::now();
    engine.render();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    engine.save(filename);
}

void test(std::function<void()> func, const std::string &filename) {
    engine.clear();
    func();
    render_and_save(filename);
}

#define TEST(func)                                                   \
    std::cout << "========= " << #func << " =========" << std::endl; \
    test(func, #func ".bmp");                                        \
    std::cout << std::endl;

int main() {
    engine.init(WIDTH, HEIGHT);

    TEST(lab_1);
    TEST(lab_2);
    TEST(lab_3);
    TEST(lab_4);

    TEST(ex_3);
    TEST(ex_4);

    TEST(perf_test);

    render_and_save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}

void lab_1() {
    // 红色虚线
    engine.set_pen_options({.color = Colors::Red, .type = PenOptions::LineType::DASH});
    engine.add_primitive(make_line({0, 0}, {799, 599}));
    // 绿色点划线
    engine.set_pen_options({.color = Colors::Green, .type = PenOptions::LineType::DASH_DOT});
    engine.add_primitive(make_line({0, 599}, {799, 0}));
    // 蓝色粗线
    engine.set_pen_options({.color = Colors::Blue, .width = 30});
    engine.add_primitive(make_line({400, 0}, {400, 599}));
    // 黄色点线
    engine.set_pen_options({.color = Colors::Yellow, .type = PenOptions::LineType::DOT});
    engine.add_primitive(make_line({0, 300}, {799, 300}));
    // 白色整圆
    engine.set_pen_options({.color = Colors::White});
    engine.add_primitive(make_circle_center_radius({400, 300}, 200));
    // 青色圆弧
    engine.set_pen_options({.color = Colors::Cyan});
    engine.add_primitive(make_arc_center_radius_angle({400, 300}, 300, 0, std::numbers::pi / 2));
}

void lab_2() {
    engine.set_pen_options({.color = Colors::Red, .fill_color = Colors::Green});
    // 矩形绘制
    engine.add_primitive(make_rectangle({100, 100}, {200, 200}));
    // 多边形绘制
    engine.add_primitive(make_polygon({{300, 100}, {400, 100}, {350, 200}}));
    engine.set_pen_options({.color = Colors::Blue, .fill_color = Colors::Yellow});
    engine.add_primitive(make_polygon({{500, 100}, {600, 100}, {650, 200}, {500, 300}}));
    // 区域填充
    engine.set_pen_options({.color = Colors::Red, .fill_color = Colors::Cyan});
    engine.add_primitive(make_fill({350, 150}));
    // 画点线展示裁剪效果
    // 红色虚线
    engine.set_pen_options({.color = Colors::Red, .type = PenOptions::LineType::DASH});
    engine.add_primitive(make_line({0, 0}, {799, 599}));
    // 绿色点划线
    engine.set_pen_options({.color = Colors::Green, .type = PenOptions::LineType::DASH_DOT});
    engine.add_primitive(make_line({0, 599}, {799, 0}));
    // 蓝色粗线
    engine.set_pen_options({.color = Colors::Blue, .width = 30});
    engine.add_primitive(make_line({400, 0}, {400, 599}));
    // 黄色点线
    engine.set_pen_options({.color = Colors::Yellow, .type = PenOptions::LineType::DOT});
    engine.add_primitive(make_line({0, 300}, {799, 300}));
    // 矩形裁剪
    engine.set_global_options({.clip = {true, make_rectangle({100, 100}, {700, 500})}});
    // 多边形裁剪
    // engine.set_global_options({.clip = {true, make_polygon({})}});
    engine.set_global_options({.clip = {true, make_polygon({{300, 100}, {400, 100}, {350, 200}})}});
}

void lab_3() {
    engine.set_pen_options({.color = Colors::Red});
    // 平移变换
    engine.add_primitive(make_translate(100, 0));
    // 三角形绘制
    engine.add_primitive(make_polygon({{100, 400}, {200, 400}, {150, 500}}));
}

void lab_4() {
    // 贝塞尔曲线绘制
    auto bezier_control_points = std::vector<Point>{{100, 100}, {200, 200}, {300, 100}, {400, 200}};
    engine.set_pen_options({.color = Colors::Red});
    // 绘制控制多边形
    engine.add_primitive(make_polygon(bezier_control_points));
    // 贝塞尔曲线绘制
    engine.set_pen_options({.color = Colors::Yellow});
    engine.add_primitive(make_bezier_curve(bezier_control_points));
    auto bspline_control_points =
        std::vector<Point>{{100, 400}, {200, 500}, {300, 400}, {400, 500}};
    auto bspline_knots = std::vector<float>{0, 1, 2, 3, 4, 5, 6, 7};
    // 绘制控制多边形
    engine.set_pen_options({.color = Colors::Red});
    engine.add_primitive(make_polygon(bspline_control_points));
    // b样条曲线绘制
    engine.set_pen_options({.color = Colors::Yellow});
    engine.add_primitive(make_bspline_curve(bspline_control_points, bspline_knots));
}

void ex_3() {
    engine.set_pen_options({.color = Colors::Red});
    // 先经过平移（平移量为（1，0）
    // 后经过旋转（旋转角度为+30度）
    // 平移变换
    engine.add_primitive(make_translate(100, 0));
    // 旋转变换
    engine.add_primitive(make_rotate(30.0 / 180.0 * std::numbers::pi, {0, 0}));
    // 三角形绘制
    engine.add_primitive(make_polygon({{100, 100}, {200, 100}, {150, 200}}));

    engine.add_primitive(make_line({0, 0}, {100, 100}));

    extern Matrix3f make_transform_matrix(const Transform &transform);

    auto t1 = make_transform_matrix(make_translate(1, 0));
    auto t2 = make_transform_matrix(make_rotate(30.0 / 180.0 * std::numbers::pi, {0, 0}));
    auto t3 = t2 * t1;
    std::cout << t3 << std::endl;
    //P（2，1）经过平移（1，0）和旋转（30度）后的坐标
    auto p1 = t3 * Vector2f(2, 1).xy1();
    std::cout << p1.xy() << std::endl;

    std::cout << "----------------" << std::endl;

    // 先经过旋转（旋转角度为+30度）后经过平移（平移量为（1，0）
    auto t4 = t1 * t2;
    std::cout << t4 << std::endl;
    //P（2，1）经过旋转（30度）和平移（1，0）后的坐标
    auto p2 = t4 * Vector2f(2, 1).xy1();
    std::cout << p2.xy() << std::endl;
}

// 将float转换为分数
void floatToFraction(float value, int &numerator, int &denominator) {
    std::function<int(int, int)> gcd = [&](int a, int b) -> int {
        return b == 0 ? a : gcd(b, a % b);
    };

    const float epsilon = 1e-6;  // 精度
    int sign = (value < 0) ? -1 : 1;
    value = fabs(value);

    denominator = 1;
    while (fabs(value * denominator - round(value * denominator)) > epsilon) {
        denominator++;
    }

    numerator = static_cast<int>(round(value * denominator)) * sign;

    // 简化分数
    int gcdVal = gcd(abs(numerator), denominator);
    numerator /= gcdVal;
    denominator /= gcdVal;
}

struct Fraction {
    int numerator;
    int denominator;

    Fraction() : numerator(0), denominator(1) {}

    Fraction(int value) : numerator(value), denominator(1) {}

    Fraction(float value) { floatToFraction(value, numerator, denominator); }

    Fraction(int numerator, int denominator) : numerator(numerator), denominator(denominator) {}

    friend std::ostream &operator<<(std::ostream &os, const Fraction &f) {
        if (f.denominator == 1) {
            os << f.numerator;
            return os;
        }
        os << f.numerator << "/" << f.denominator;
        return os;
    }
};

Fraction as_fraction(float value) {
    return Fraction(value);
}

template <size_t N>
Vector<N, Fraction> as_fraction(const Vector<N, float> &v) {
    Vector<N, Fraction> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = Fraction(v[i]);
    }
    return result;
}

template <size_t M, size_t N>
Matrix<M, N, Fraction> as_fraction(const Matrix<M, N, float> &m) {
    Matrix<M, N, Fraction> result;
    for (size_t i = 0; i < M; i++) {
        for (size_t j = 0; j < N; j++) {
            result[i][j] = Fraction(m[i][j]);
        }
    }
    return result;
}

void ex_4() {
    std::cout << "---question 1---" << std::endl;
    std::vector<Vector2f> p = {{1, 1}, {3, 4}, {5, -1}, {8, 1}};

    Matrix<2, 4, float> G = Matrix<4, 2, float>({{1, 1}, {3, 4}, {5, -1}, {8, 1}}).transpose();
    Matrix<4, 4, float> M = {{1, -3, 3, -1}, {0, 3, -6, 3}, {0, 0, 3, -3}, {0, 0, 0, 1}};

    std::cout << "几何矩阵G_BEZ:\n" << G << std::endl;
    std::cout << "基函数矩阵M_BEZ:\n" << M << std::endl;

    auto q1 = [&](float t) {
        std::cout << "t = " << as_fraction(t) << ": ";

        Matrix<4, 1, float> T = Matrix<1, 4, float>({{1, t, t * t, t * t * t}}).transpose();

        std::cout << as_fraction((G * M * T).transpose()) << std::endl;
    };
    q1(0.25);
    q1(0.5);

    std::cout << "---question 2---" << std::endl;

    auto de_casteljau = [&](const std::vector<Vector2f> &points, float t, int n = 1) -> Vector2f {
        std::function<Vector2f(const std::vector<Vector2f> &, float, int)> de_casteljau =
            [&](const std::vector<Vector2f> &points, float t, int n) -> Vector2f {
            if (points.size() == 1) {
                return points[0];
            }
            std::vector<Vector2f> new_points;
            for (size_t i = 0; i < points.size() - 1; i++) {
                auto p = (1 - t) * points[i] + t * points[i + 1];
                std::cout << "P^" << n << "_" << i << " = " << as_fraction(p) << std::endl;
                new_points.push_back(p);
            }
            return de_casteljau(new_points, t, n + 1);
        };
        return de_casteljau(points, t, n);
    };

    de_casteljau(p, 0.5);
}

void perf_test() {
    constexpr int N = 10000;

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());

    // 生成随机颜色
    auto random_color = [&gen]() {
        std::uniform_int_distribution<int> dis(0, 255);
        return Color{dis(gen) / 255.0f, dis(gen) / 255.0f, dis(gen) / 255.0f, 1};
    };

    // 生成随机在画布内的点
    auto random_point = [&gen]() {
        std::uniform_int_distribution<int> dis(0, WIDTH - 1);
        std::uniform_int_distribution<int> dis2(0, HEIGHT - 1);
        const auto x = dis(gen);
        const auto y = dis2(gen);
        assert(x >= 0 && x < WIDTH);
        assert(y >= 0 && y < HEIGHT);
        return Point{x, y};
    };

    // 生成随机直线
    auto random_line = [&random_point]() { return make_line(random_point(), random_point()); };

    // 生成随机矩形
    auto random_rectangle = [&random_point]() {
        return make_rectangle(random_point(), random_point());
    };

    // 生成随机圆
    auto random_circle = [&gen, &random_point]() {
        std::uniform_int_distribution<int> dis(0, 200);
        return make_circle_center_radius(random_point(), dis(gen));
    };

    // 生成随机变换
    auto random_transform = [&gen, &random_point]() {
        // 随机类型
        std::uniform_int_distribution<int> dis(0, 2);
        // 随机数
        std::uniform_real_distribution<float> dis2(0, 1);
        switch (dis(gen)) {
            case 0:
                return Transform{make_translate(dis2(gen) * 100, dis2(gen) * 100)};
            case 1:
                return Transform{make_rotate(dis2(gen) * std::numbers::pi, random_point())};
            case 2:
                return Transform{make_scale(dis2(gen) * 2, dis2(gen) * 2, random_point())};
        }
        return Transform{};
    };

    // 生成随机贝塞尔曲线
    auto random_bezier_curve = [&gen, &random_point]() {
        std::uniform_int_distribution<int> dis(2, 10);
        std::vector<Point> points;
        for (int i = 0; i < dis(gen); i++) {
            points.push_back(random_point());
        }
        return make_bezier_curve(points);
    };

    // 生成随机图元
    auto random_primitive = [&]() {
        // 随机类型
        std::uniform_int_distribution<int> dis(0, 4);
        switch (dis(gen)) {
            case 0:
                return Primitive{random_line()};
            case 1:
                return Primitive{random_rectangle()};
            case 2:
                return Primitive{random_circle()};
            case 3:
                return Primitive{random_bezier_curve()};
            case 4:
                return Primitive{random_transform()};
        }
        return Primitive{};
    };

    for (int i = 0; i < N; i++) {
        engine.set_pen_options({.color = random_color(), .fill_color = random_color()});
        engine.add_primitive(random_primitive());
    }
}
