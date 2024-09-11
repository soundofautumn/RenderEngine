//
// Created by Autumn Sound on 2024/9/9.
//

#ifndef RENDERENGINE_HANDLE_REQUEST_H
#define RENDERENGINE_HANDLE_REQUEST_H

#include <string>
#include "engine.hpp"

using RenderCore::RenderEngine;

void handle_engine_request(const std::string &message, RenderEngine &engine);

#endif //RENDERENGINE_HANDLE_REQUEST_H
