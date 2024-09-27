//
// Created by Autumn Sound on 2024/9/20.
//
#ifndef RENDERENGINE_SERIALIZE_POLYGON_H
#define RENDERENGINE_SERIALIZE_POLYGON_H

#include <boost/json.hpp>

#include "polygon.hpp"
#include "serialize_point.h"

inline RenderCore::Polygon deserialize_polygon(const boost::json::object &obj) {
    RenderCore::Polygon polygon;
    for (const auto &point : obj.at("points").as_array()) {
        polygon.push_back(deserialize_point(point.as_object()));
    }
    return polygon;
}

inline boost::json::object serialize_polygon(const RenderCore::Polygon &polygon) {
    boost::json::array points;
    for (const auto &point : polygon) {
        points.push_back(serialize_point(point));
    }
    return {{"points", points}};
}

#endif
