//
// Created by Autumn Sound on 2024/9/18.
//

#ifndef RENDERENGINE_SERIALIZE_CIRCLE_H
#define RENDERENGINE_SERIALIZE_CIRCLE_H

#include <boost/json.hpp>

#include "circle.hpp"
#include "serialize_options.h"
#include "serialize_point.h"

boost::json::object serialize_circle(const RenderCore::Circle &circle) {
    if (std::holds_alternative<RenderCore::CircleUseCenterRadius>(circle)) {
        const auto &circle_ = std::get<RenderCore::CircleUseCenterRadius>(circle);
        return {{"type", "center_radius"}, {"center", serialize_point(circle_.center)},
            {"radius", circle_.radius}};
    } else if (std::holds_alternative<RenderCore::CircleUseThreePoints>(circle)) {
        const auto &circle_ = std::get<RenderCore::CircleUseThreePoints>(circle);
        return {{"type", "three_points"}, {"p1", serialize_point(circle_.p1)},
            {"p2", serialize_point(circle_.p2)}, {"p3", serialize_point(circle_.p3)}};
    }
    return {};
}

RenderCore::Circle deserialize_circle(const boost::json::object &obj) {
    if (obj.at("type").as_string() == "center_radius") {
        return RenderCore::CircleUseCenterRadius{
            .center = deserialize_point(obj.at("center").as_object()),
            .radius = static_cast<int>(obj.at("radius").as_int64())};
    } else if (obj.at("type").as_string() == "three_points") {
        return RenderCore::CircleUseThreePoints{.p1 = deserialize_point(obj.at("p1").as_object()),
            .p2 = deserialize_point(obj.at("p2").as_object()),
            .p3 = deserialize_point(obj.at("p3").as_object())};
    }
    return {};
}

boost::json::object serialize_arc(const RenderCore::Arc &arc) {
    if (std::holds_alternative<RenderCore::ArcUseCenterRadiusAngle>(arc)) {
        const auto &arc_ = std::get<RenderCore::ArcUseCenterRadiusAngle>(arc);
        return {{"type", "center_radius_angle"}, {"center", serialize_point(arc_.center)},
            {"radius", arc_.radius}, {"start_angle", arc_.start_angle},
            {"end_angle", arc_.end_angle}};
    } else if (std::holds_alternative<RenderCore::ArcUseThreePoints>(arc)) {
        const auto &arc_ = std::get<RenderCore::ArcUseThreePoints>(arc);
        return {{"type", "three_points"}, {"p1", serialize_point(arc_.p1)},
            {"p2", serialize_point(arc_.p2)}, {"p3", serialize_point(arc_.p3)}};
    }
    return {};
}

RenderCore::Arc deserialize_arc(const boost::json::object &obj) {
    if (obj.at("type").as_string() == "center_radius_angle") {
        return RenderCore::ArcUseCenterRadiusAngle{
            .center = deserialize_point(obj.at("center").as_object()),
            .radius = static_cast<int>(obj.at("radius").as_int64()),
            .start_angle = static_cast<float>(obj.at("start_angle").as_double()),
            .end_angle = static_cast<float>(obj.at("end_angle").as_double())};
    } else if (obj.at("type").as_string() == "three_points") {
        return RenderCore::ArcUseThreePoints{.p1 = deserialize_point(obj.at("p1").as_object()),
            .p2 = deserialize_point(obj.at("p2").as_object()),
            .p3 = deserialize_point(obj.at("p3").as_object())};
    }
    return {};
}

#endif
