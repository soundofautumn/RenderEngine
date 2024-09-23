//
// Created by Autumn Sound on 2024/9/18.
//

#ifndef RENDERENGINE_SERIALIZE_COLOR_H
#define RENDERENGINE_SERIALIZE_COLOR_H

#include <boost/json.hpp>

#include "color.hpp"

boost::json::object serialize_color(const RenderCore::Color &color) {
    return {{"r", static_cast<int64_t>(color.r * 255)}, {"g", static_cast<int64_t>(color.g * 255)},
        {"b", static_cast<int64_t>(color.b * 255)}, {"a", static_cast<int64_t>(color.a * 255)}};
}

RenderCore::Color deserialize_color(const boost::json::object &obj) {
    return RenderCore::Color{static_cast<float>(obj.at("r").as_int64()) / 255.0f,
        static_cast<float>(obj.at("g").as_int64()) / 255.0f,
        static_cast<float>(obj.at("b").as_int64()) / 255.0f,
        static_cast<float>(obj.at("a").as_int64()) / 255.0f};
}

#endif
