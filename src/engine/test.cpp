//
// Created by Autumn Sound on 2024/9/5.
//
#include <numbers>

#include "color.hpp"
#include "engine.hpp"

using namespace RenderCore;

static RenderEngine engine;

void lab_1();
void lab_2();

int main() {
    engine.init(800, 600);
    engine.clear();

    lab_1();
    lab_2();

    engine.render();
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}

void lab_1() {
    engine.set_pen_options({.color = Colors::Red, .type = PenOptions::LineType::DASH});
    engine.add_primitive(make_line({0, 0}, {799, 599}));
    engine.set_pen_options({.color = Colors::Green, .type = PenOptions::LineType::DASH_DOT});
    engine.add_primitive(make_line({0, 599}, {799, 0}));
    engine.set_pen_options({.color = Colors::Blue, .width = 30});
    engine.add_primitive(make_line({400, 0}, {400, 599}));
    engine.set_pen_options({.color = Colors::Yellow, .type = PenOptions::LineType::DOT});
    engine.add_primitive(make_line({0, 300}, {799, 300}));
    engine.set_pen_options({.color = Colors::White});
    engine.add_primitive(make_circle_center_radius({400, 300}, 200));
    engine.set_pen_options({.color = Colors::Cyan});
    engine.add_primitive(make_arc_center_radius_angle({400, 300}, 200, 0, std::numbers::pi / 2));
}

void lab_2() {
    engine.set_pen_options({.color = Colors::Red, .fill_color = Colors::Green});
    // 矩形绘制
    engine.add_primitive(make_rectangle({100, 100}, {200, 200}, Rectangle::Action::Draw));
    // 多边形绘制
    engine.add_primitive(make_polygon({{300, 100}, {400, 100}, {350, 200}}, Polygon::Action::Draw));
    engine.set_pen_options({.color = Colors::Blue, .fill_color = Colors::Yellow});
    engine.add_primitive(make_polygon({{500, 100}, {600, 100}, {650, 200}, {500, 300}, {500, 200}}, Polygon::Action::Draw));
    // 区域填充
    // engine.add_primitive(make_polygon({{500, 100}, {600, 100}, {550, 200}}, Polygon::Action::Clip));
    // 矩形裁剪
    // engine.add_primitive(make_rectangle({100, 300}, {200, 400}, Rectangle::Action::Clip));
}
