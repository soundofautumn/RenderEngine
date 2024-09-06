//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_LINE_H
#define RENDERENGINE_LINE_H

#include "bitmap.h"
#include "options.h"
#include "engine.h"


using Point = RenderEngine::Point;

void draw_line_by_dda(RenderEngine *engine,  const Point &p1, const Point &p2, const PenOptions &options);

void draw_line_by_midpoint(RenderEngine *engine,  const Point &p1, const Point &p2, const PenOptions &options);

void draw_line_by_bresenham(RenderEngine *engine,  const Point &p1, const Point &p2, const PenOptions &options);


#endif //RENDERENGINE_LINE_H
