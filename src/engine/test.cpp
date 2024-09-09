//
// Created by Autumn Sound on 2024/9/5.
//
#include "engine.h"
#include "colors.hpp"

int main() {
    RenderEngine engine;
    engine.init(800, 600);
    engine.clear();
    engine.draw_line({0, 0}, {799, 599}, {.color = Colors::Red});
    engine.draw_line({0, 599}, {799, 0}, {.color = Colors::Green});
    engine.draw_line({400, 0}, {400, 599}, {.color = Colors::Blue});
    engine.draw_line({0, 300}, {799, 300}, {.color = Colors::Yellow});
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}