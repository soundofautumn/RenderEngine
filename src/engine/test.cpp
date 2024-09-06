//
// Created by Autumn Sound on 2024/9/5.
//
#include "engine.h"
#include "colors.hpp"

int main() {
    RenderEngine engine;
    engine.init(800, 600);
    engine.clear();
    engine.draw_pixel(100, 100, {1, 0, 0, 1});
    engine.draw_line({1, 1}, {200, 200}, {.color = Colors::Red});
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}