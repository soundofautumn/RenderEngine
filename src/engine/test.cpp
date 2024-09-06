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
    engine.save("output.bmp");
#ifdef _WIN32
    system("mspaint output.bmp");
#endif
    return 0;
}