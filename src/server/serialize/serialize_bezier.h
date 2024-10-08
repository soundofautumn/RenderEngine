//
// Created by Autumn Sound on 2024/9/30.
//
#ifndef RENDERENGINE_SERIALIZE_BEZIER_H
#define RENDERENGINE_SERIALIZE_BEZIER_H

#include <boost/json.hpp>

#include "bezier.hpp"
#include "serialize_point.h"

inline RenderCore::BezierCurve deserialize_bezier_curve(const boost::json::object &obj) {
    RenderCore::BezierCurve curve;
    for (const auto &point : obj.at("control_points").as_array()) {
        curve.push_back(deserialize_point(point.as_object()));
    }
    return curve;
}

inline boost::json::object serialize_bezier_curve(const RenderCore::BezierCurve &curve) {
    boost::json::array control_points;
    for (const auto &point : curve) {
        control_points.push_back(serialize_point(point));
    }
    return {{"control_points", control_points}};
}

inline RenderCore::BsplineCurve deserialize_bspline_curve(const boost::json::object &obj) {
    RenderCore::BsplineCurve curve;
    for (const auto &point : obj.at("control_points").as_array()) {
        curve.control_points.push_back(deserialize_point(point.as_object()));
    }
    for (const auto &knot : obj.at("knots").as_array()) {
        curve.knots.push_back(knot.as_double());
    }
    return curve;
}

inline boost::json::object serialize_bspline_curve(const RenderCore::BsplineCurve &curve) {
    boost::json::array control_points;
    for (const auto &point : curve.control_points) {
        control_points.push_back(serialize_point(point));
    }
    boost::json::array knots;
    for (const auto &knot : curve.knots) {
        knots.push_back(knot);
    }
    return {{"control_points", control_points}, {"knots", knots}};
}

#endif
