//
// Created by Autumn Sound on 2024/9/23.
//

#ifndef RENDERENGINE_SERIALIZE_CLIP_H
#define RENDERENGINE_SERIALIZE_CLIP_H

#include <boost/json.hpp>

#include "fill.hpp"
#include "serialize_polygon.h"
#include "serialize_rectangle.h"

using namespace RenderCore;

boost::json::object serialize_clip_window(const std::variant<Rectangle, Polygon> &window) {
    if (std::holds_alternative<Rectangle>(window)) {
        return {{"Rectangle", serialize_rectangle(std::get<Rectangle>(window))}};
    } else if (std::holds_alternative<Polygon>(window)) {
        return {{"Polygon", serialize_polygon(std::get<Polygon>(window))}};
    }
    return {};
}

std::variant<Rectangle, Polygon> deserialize_clip_window(const boost::json::object &obj) {
    if (obj.contains("Rectangle")) {
        return deserialize_rectangle(obj.at("Rectangle").as_object());
    } else if (obj.contains("Polygon")) {
        return deserialize_polygon(obj.at("Polygon").as_object());
    }
    return {};
}

boost::json::object serialize_clip(const Clip &clip) {
    return {{"enable", clip.enable}, {"window", serialize_clip_window(clip.window)}};
}

Clip deserialize_clip(const boost::json::object &obj) {
    return Clip{.enable = obj.at("enable").as_bool(),
        .window = deserialize_clip_window(obj.at("window").as_object())};
}

#endif
