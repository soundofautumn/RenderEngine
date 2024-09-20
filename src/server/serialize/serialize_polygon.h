//
// Created by Autumn Sound on 2024/9/20.
//
#ifndef RENDERENGINE_SERIALIZE_POLYGON_H
#define RENDERENGINE_SERIALIZE_POLYGON_H

#include <boost/json.hpp>

#include "polygon.hpp"

using namespace RenderCore;

Polygon::Action deserialize_polygon_action(const std::string &action) {
    if (action == "Draw") {
        return Polygon::Action::Draw;
    } else if (action == "Fill") {
        return Polygon::Action::Fill;
    } else if (action == "Clip") {
        return Polygon::Action::Clip;
    }
    return Polygon::Action::Draw;
}

std::string serialize_polygon_action(Polygon::Action action) {
    switch (action) {
        case Polygon::Action::Draw:
            return "Draw";
        case Polygon::Action::Fill:
            return "Fill";
        case Polygon::Action::Clip:
            return "Clip";
    }
    return "Draw";
}

Polygon deserialize_polygon(const boost::json::object &obj) {
    Polygon polygon;
    for (const auto &point : obj.at("points").as_array()) {
        polygon.points.push_back(deserialize_point(point.as_object()));
    }
    polygon.action = deserialize_polygon_action(obj.at("action").as_string().c_str());
    return polygon;
}

boost::json::object serialize_polygon(const Polygon &polygon) {
    boost::json::array points;
    for (const auto &point : polygon.points) {
        points.push_back(serialize_point(point));
    }
    return {{"points", points}, {"action", serialize_polygon_action(polygon.action)}};
}

#endif
