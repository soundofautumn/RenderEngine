//
// Created by Autumn Sound on 2024/9/12.
//

#include "serialize.h"

#include "serialize_circle.h"
#include "serialize_color.h"
#include "serialize_line.h"
#include "serialize_point.h"
#include "serialize_polygon.h"
#include "serialize_rectangle.h"

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
    } else if (primitive.contains("Polygon")) {
        return deserialize_polygon(primitive.at("Polygon").as_object());
    } else if (primitive.contains("Rectangle")) {
        return deserialize_rectangle(primitive.at("Rectangle").as_object());
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
    } else if (std::holds_alternative<RenderCore::Polygon>(primitive)) {
        return {{"Polygon", serialize_polygon(std::get<RenderCore::Polygon>(primitive))}};
    } else if (std::holds_alternative<RenderCore::Rectangle>(primitive)) {
        return {{"Rectangle", serialize_rectangle(std::get<RenderCore::Rectangle>(primitive))}};
    }
    return {};
}
