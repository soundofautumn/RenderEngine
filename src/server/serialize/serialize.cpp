//
// Created by Autumn Sound on 2024/9/12.
//

#include "serialize.h"

#include "serialize_circle.h"
#include "serialize_color.h"
#include "serialize_line.h"
#include "serialize_point.h"

using namespace RenderCore;

Primitive deserialize_primitive(const boost::json::object &primitive) {
    if (primitive.contains("Line")) {
        return deserialize_line(primitive.at("Line").as_object());
    } else if (primitive.contains("Circle")) {
        return deserialize_circle(primitive.at("Circle").as_object());
    } else if (primitive.contains("Arc")) {
        return deserialize_arc(primitive.at("Arc").as_object());
    } else if (primitive.contains("PenOptions")) {
        return deserialize_pen_options(primitive.at("PenOptions").as_object());
    }
    return {};
}

boost::json::object serialize_primitive(const Primitive &primitive) {
    if (std::holds_alternative<RenderCore::Line>(primitive)) {
        return {{"Line", serialize_line(std::get<RenderCore::Line>(primitive))}};
    } else if (std::holds_alternative<RenderCore::Circle>(primitive)) {
        return {{"Circle", serialize_circle(std::get<RenderCore::Circle>(primitive))}};
    } else if (std::holds_alternative<RenderCore::Arc>(primitive)) {
        return {{"Arc", serialize_arc(std::get<RenderCore::Arc>(primitive))}};
    } else if (std::holds_alternative<RenderCore::PenOptions>(primitive)) {
        return {{"PenOptions", serialize_pen_options(std::get<RenderCore::PenOptions>(primitive))}};
    }
    return {};
}
