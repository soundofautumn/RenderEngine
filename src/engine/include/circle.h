//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_CIRCLE_H
#define RENDERENGINE_CIRCLE_H

#include "engine.h"
#include "options.h"

using Point = RenderEngine::Point;

void draw_circle_by_midpoint(RenderEngine *engine, const Point &center, int radius, const PenOptions &options);

void draw_arc_by_midpoint(RenderEngine *engine, const Point &center, int radius, float start_angle, float end_angle,
                          const PenOptions &options);

#endif //RENDERENGINE_CIRCLE_H
