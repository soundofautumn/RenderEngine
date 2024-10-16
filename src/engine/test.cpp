//
// Created by Autumn Sound on 2024/9/5.
//
#include <chrono>
#include <functional>
#include <iostream>
#include <numbers>
#include <ostream>
#include <vector>

#include "color.hpp"
#include "engine.hpp"
#include "line.hpp"
#include "matrix.hpp"
#include "point.hpp"
#include "rectangle.hpp"
#include "transform.hpp"
#include "vector.hpp"

using namespace RenderCore;

static RenderEngine engine;

void lab_1();
void lab_2();
void lab_3();
void lab_4();

void ex_3();
void ex_4();

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
    engine.init(800, 600);

    TEST(lab_1);
    TEST(lab_2);
    TEST(lab_3);
    TEST(lab_4);

    TEST(ex_3);
    TEST(ex_4);

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
}

void lab_3() {
    engine.set_pen_options({.color = Colors::Red});
    // 平移变换
    engine.add_primitive(make_translate(100, 0));
    // 三角形绘制
    engine.add_primitive(make_polygon({{100, 400}, {200, 400}, {150, 500}}));
}

void lab_4() {
    engine.set_pen_options({.color = Colors::Yellow});
    // 贝塞尔曲线绘制
    engine.add_primitive(make_bezier_curve({{100, 100}, {200, 200}, {300, 100}, {400, 200}}));
    // b样条曲线绘制
    engine.add_primitive(make_bspline_curve({{100, 400}, {200, 500}, {300, 400}, {400, 500}},
                                           {0, 0, 0, 1, 2, 3, 3, 3}));
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
