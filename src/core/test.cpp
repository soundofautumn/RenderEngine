//
// Created by Autumn Sound on 2024/9/5.
//
#include "engine.h"

int main() {
    RenderEngine engine;
    engine.init(800, 600);
    engine.clear();
    engine.draw_pixel(100, 100, {1, 0, 0, 1});
    engine.draw_line({1, 1}, {200, 200}, {0, 1, 0, 1});
    engine.save("output.bmp");
    return 0;

}