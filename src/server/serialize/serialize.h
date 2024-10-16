//
// Created by Autumn Sound on 2024/9/12.
//

#ifndef RENDERENGINE_SERIALIZE_H
#define RENDERENGINE_SERIALIZE_H

#include <boost/json/src.hpp>

#include "primitive.hpp"
#include "serialize_bezier.h"
#include "serialize_circle.h"
#include "serialize_fill.h"
#include "serialize_line.h"
#include "serialize_options.h"
#include "serialize_polygon.h"
#include "serialize_rectangle.h"
#include "serialize_transform.h"

inline RenderCore::Primitive deserialize_primitive(const boost::json::object &primitive) {
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
    } else if (primitive.contains("Fill")) {
        return deserialize_fill(primitive.at("Fill").as_object());
    } else if (primitive.contains("Transform")) {
        return deserialize_transform(primitive.at("Transform").as_object());
    } else if (primitive.contains("BezierCurve")) {
        return deserialize_bezier_curve(primitive.at("BezierCurve").as_object());
    } else if (primitive.contains("BsplineCurve")) {
        return deserialize_bspline_curve(primitive.at("BsplineCurve").as_object());
    }
    return {};
}

inline boost::json::object serialize_primitive(const RenderCore::Primitive &primitive) {

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
    } else if (std::holds_alternative<RenderCore::Fill>(primitive)) {
        return {{"Fill", serialize_fill(std::get<RenderCore::Fill>(primitive))}};
    } else if (std::holds_alternative<RenderCore::Transform>(primitive)) {
        return {{"Transform", serialize_transform(std::get<RenderCore::Transform>(primitive))}};
    } else if (std::holds_alternative<RenderCore::BezierCurve>(primitive)) {
        return {{"BezierCurve",
            serialize_bezier_curve(std::get<RenderCore::BezierCurve>(primitive))}};
    } else if (std::holds_alternative<RenderCore::BsplineCurve>(primitive)) {
        return {{"BsplineCurve",
            serialize_bspline_curve(std::get<RenderCore::BsplineCurve>(primitive))}};
    }
    return {};
}

#endif  //RENDERENGINE_SERIALIZE_H
