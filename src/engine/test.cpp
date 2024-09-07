//
// Created by Autumn Sound on 2024/9/5.
//
#include "engine.h"
#include "colors.hpp"

int main() {
    RenderEngine engine;
    engine.init(800, 600);
    engine.clear();
    engine.draw_line({0, 0}, {799, 599}, {.color = Colors::Red}, LineAlgorithm::DDA);
    engine.draw_line({0, 599}, {799, 0}, {.color = Colors::Green}, LineAlgorithm::DDA);
    engine.draw_line({400, 0}, {400, 599}, {.color = Colors::Blue}, LineAlgorithm::DDA);
    engine.draw_line({0, 300}, {799, 300}, {.color = Colors::Yellow}, LineAlgorithm::DDA);
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}