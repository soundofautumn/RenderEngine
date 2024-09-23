//
// Created by Autumn Sound on 2024/9/20.
//
#ifndef RENDERENGINE_SERIALIZE_POLYGON_H
#define RENDERENGINE_SERIALIZE_POLYGON_H

#include <boost/json.hpp>

#include "polygon.hpp"
#include "serialize_point.h"

using namespace RenderCore;

Polygon deserialize_polygon(const boost::json::object &obj) {
    Polygon polygon;
    for (const auto &point : obj.at("points").as_array()) {
        polygon.points.push_back(deserialize_point(point.as_object()));
    }
    return polygon;
}

boost::json::object serialize_polygon(const Polygon &polygon) {
    boost::json::array points;
    for (const auto &point : polygon.points) {
        points.push_back(serialize_point(point));
    }
    return {{"points", points}};
}

#endif
