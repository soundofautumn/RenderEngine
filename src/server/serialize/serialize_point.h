//
// Created by Autumn Sound on 2024/9/18.
//

#ifndef RENDERENGINE_SERIALIZE_POINT_H
#define RENDERENGINE_SERIALIZE_POINT_H

#include <boost/json.hpp>

#include "point.hpp"

using namespace RenderCore;

Point deserialize_point(const boost::json::object &obj) {
    return Point{
        static_cast<int>(obj.at("x").as_int64()), static_cast<int>(obj.at("y").as_int64())};
}

boost::json::object serialize_point(const Point &point) {
    return {{"x", point.x}, {"y", point.y}};
}

#endif
