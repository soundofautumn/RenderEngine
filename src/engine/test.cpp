//
// Created by Autumn Sound on 2024/9/5.
//
#include <numbers>

#include "color.hpp"
#include "engine.hpp"

using namespace RenderCore;

int main() {
    RenderEngine engine;
    engine.init(800, 600);
    engine.clear();
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
    engine.render();
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}
