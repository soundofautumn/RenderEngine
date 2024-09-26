//
// Created by Autumn Sound on 2024/9/18.
//
#ifndef RENDERENGINE_SERIALIZE_OPTIONS_H
#define RENDERENGINE_SERIALIZE_OPTIONS_H

#include <boost/json.hpp>

#include "options.hpp"
#include "serialize_clip.h"
#include "serialize_color.h"

inline boost::json::object serialize_pen_options(const RenderCore::PenOptions &options) {
    return {{"color", serialize_color(options.color)},
        {"fill_color", serialize_color(options.fill_color)}, {"width", options.width},
        {"type", static_cast<int64_t>(options.type)}, {"dash", options.dash}};
}

inline RenderCore::PenOptions deserialize_pen_options(const boost::json::object &obj) {
    return RenderCore::PenOptions{.color = deserialize_color(obj.at("color").as_object()),
        .fill_color = deserialize_color(obj.at("fill_color").as_object()),
        .width = static_cast<int>(obj.at("width").as_int64()),
        .type = static_cast<RenderCore::PenOptions::LineType>(obj.at("type").as_int64()),
        .dash = static_cast<int>(obj.at("dash").as_int64())};
}

inline boost::json::object serialize_global_options(const RenderCore::GlobalOptions &options) {
    return {{"background_color", serialize_color(options.background_color)},
        {"clip", serialize_clip(options.clip)}};
}

inline RenderCore::GlobalOptions deserialize_global_options(const boost::json::object &obj) {
    return RenderCore::GlobalOptions{
        .background_color = deserialize_color(obj.at("background_color").as_object()),
        .clip = deserialize_clip(obj.at("clip").as_object())};
}

#endif
