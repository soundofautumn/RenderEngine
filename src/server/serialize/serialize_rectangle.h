//
// Created by Autumn Sound on 2024/9/20.
//
#ifndef RENDERENGINE_SERIALIZE_RECTANGLE_H
#define RENDERENGINE_SERIALIZE_RECTANGLE_H

#include <boost/json.hpp>

#include "rectangle.hpp"

using namespace RenderCore;

Rectangle deserialize_rectangle(const boost::json::object &obj) {
    Rectangle rectangle;
    rectangle.top_left = deserialize_point(obj.at("top_left").as_object());
    rectangle.bottom_right = deserialize_point(obj.at("bottom_right").as_object());
    rectangle.action = static_cast<Rectangle::Action>(obj.at("action").as_int64());
    return rectangle;
}

boost::json::object serialize_rectangle(const Rectangle &rectangle) {
    return {{"top_left", serialize_point(rectangle.top_left)},
        {"bottom_right", serialize_point(rectangle.bottom_right)},
        {"action", static_cast<int64_t>(rectangle.action)}};
}

#endif
