//
// Created by Autumn Sound on 2024/9/20.
//
#ifndef RENDERENGINE_SERIALIZE_RECTANGLE_H
#define RENDERENGINE_SERIALIZE_RECTANGLE_H

#include <boost/json.hpp>

#include "rectangle.hpp"
#include "serialize_point.h"

inline RenderCore::Rectangle deserialize_rectangle(const boost::json::object &obj) {
    RenderCore::Rectangle rectangle;
    rectangle.top_left = deserialize_point(obj.at("top_left").as_object());
    rectangle.bottom_right = deserialize_point(obj.at("bottom_right").as_object());
    return rectangle;
}

inline boost::json::object serialize_rectangle(const RenderCore::Rectangle &rectangle) {
    return {
        {"top_left", serialize_point(rectangle.top_left)},
        {"bottom_right", serialize_point(rectangle.bottom_right)},
    };
}

#endif
