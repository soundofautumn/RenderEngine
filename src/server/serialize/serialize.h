//
// Created by Autumn Sound on 2024/9/12.
//

#ifndef RENDERENGINE_SERIALIZE_H
#define RENDERENGINE_SERIALIZE_H

#include <boost/json.hpp>

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
    using namespace RenderCore;
    if (std::holds_alternative<Line>(primitive)) {
        return {{"Line", serialize_line(std::get<Line>(primitive))}};
    } else if (std::holds_alternative<Circle>(primitive)) {
        return {{"Circle", serialize_circle(std::get<Circle>(primitive))}};
    } else if (std::holds_alternative<Arc>(primitive)) {
        return {{"Arc", serialize_arc(std::get<Arc>(primitive))}};
    } else if (std::holds_alternative<PenOptions>(primitive)) {
        return {{"PenOptions", serialize_pen_options(std::get<PenOptions>(primitive))}};
    } else if (std::holds_alternative<Polygon>(primitive)) {
        return {{"Polygon", serialize_polygon(std::get<Polygon>(primitive))}};
    } else if (std::holds_alternative<Rectangle>(primitive)) {
        return {{"Rectangle", serialize_rectangle(std::get<Rectangle>(primitive))}};
    } else if (std::holds_alternative<Fill>(primitive)) {
        return {{"Fill", serialize_fill(std::get<Fill>(primitive))}};
    } else if (std::holds_alternative<Transform>(primitive)) {
        return {{"Transform", serialize_transform(std::get<Transform>(primitive))}};
    } else if (std::holds_alternative<BezierCurve>(primitive)) {
        return {{"BezierCurve", serialize_bezier_curve(std::get<BezierCurve>(primitive))}};
    } else if (std::holds_alternative<BsplineCurve>(primitive)) {
        return {{"BsplineCurve", serialize_bspline_curve(std::get<BsplineCurve>(primitive))}};
    }
    return {};
}

#endif  //RENDERENGINE_SERIALIZE_H
