//
// Created by Autumn Sound on 2024/9/12.
//
#include "serialize.h"

using namespace RenderCore;

Point deserialize_point(const boost::json::object &obj) {
    return Point{
            static_cast<int>(obj.at("x").as_int64()),
            static_cast<int>(obj.at("y").as_int64())
    };
}

Color deserialize_color(const boost::json::object &obj) {
    return Color{
            static_cast<float>(obj.at("r").as_int64()) / 255.0f,
            static_cast<float>(obj.at("g").as_int64()) / 255.0f,
            static_cast<float>(obj.at("b").as_int64()) / 255.0f,
            static_cast<float>(obj.at("a").as_int64()) / 255.0f
    };
}

boost::json::object serialize_color(const Color &color) {
    return {
            {"r", static_cast<int64_t>(color.r * 255)},
            {"g", static_cast<int64_t>(color.g * 255)},
            {"b", static_cast<int64_t>(color.b * 255)},
            {"a", static_cast<int64_t>(color.a * 255)}
    };
}

Primitive deserialize_primitive(const boost::json::object &primitive) {
    if (primitive.contains("Line")) {
        auto line = primitive.at("Line").as_object();
        auto p1 = line.at("p1").as_object();
        auto p2 = line.at("p2").as_object();
        auto color = line.at("color").as_object();
        auto algorithm = line.at("algorithm").as_int64();
        return RenderCore::Line{
                .p1 = deserialize_point(p1),
                .p2 = deserialize_point(p2),
                .options = {
                        .color = deserialize_color(color)
                },
                .algorithm = static_cast<RenderCore::Line::LineAlgorithm>(algorithm)
        };

    } else if (primitive.contains("Circle")) {
        auto circle = primitive.at("Circle").as_object();
        auto center = circle.at("center").as_object();
        auto radius = circle.at("radius").as_int64();
        auto color = circle.at("color").as_object();
        return RenderCore::CircleUseCenterRadius{
                .center =deserialize_point(center),
                .radius = static_cast<int>(radius),
                .options={
                        .color = deserialize_color(color)
                }
        };
    } else if (primitive.contains("Arc")) {
        auto arc = primitive.at("Arc").as_object();
        auto center = arc.at("center").as_object();
        auto radius = arc.at("radius").as_int64();
        auto start_angle = arc.at("start_angle").as_double();
        auto end_angle = arc.at("end_angle").as_double();
        auto color = arc.at("color").as_object();
        return RenderCore::ArcUseCenterRadiusAngle{
                .center = deserialize_point(center),
                .radius = static_cast<int>(radius),
                .start_angle = static_cast<float>(start_angle),
                .end_angle = static_cast<float>(end_angle),
                .options={
                        .color = deserialize_color(color)
                }
        };
    }
    return {};
}

boost::json::object serialize_primitive(const Primitive &primitive) {
    if (std::holds_alternative<RenderCore::Line>(primitive)) {
        const auto &line = std::get<RenderCore::Line>(primitive);
        return {
                {"Line", {
                        {"p1", {{"x", line.p1.x}, {"y", line.p1.y}}},
                        {"p2", {{"x", line.p2.x}, {"y", line.p2.y}}},
                        {"color", serialize_color(line.options.color)},
                        {"algorithm", static_cast<int64_t>(line.algorithm)}
                }}
        };
    } else if (std::holds_alternative<RenderCore::Circle>(primitive)) {
        const auto &circle = std::get<RenderCore::Circle>(primitive);
        if (std::holds_alternative<CircleUseCenterRadius>(circle)) {
            const auto &circle_ = std::get<CircleUseCenterRadius>(circle);
            return {
                    {"Circle", {
                            {"center", {{"x", circle_.center.x}, {"y", circle_.center.y}},
                             {"radius", circle_.radius},
                             {"color", serialize_color(circle_.options.color)}
                            }}
                    }
            };
        };
    } else if (std::holds_alternative<RenderCore::Arc>(primitive)) {
        const auto &arc = std::get<RenderCore::Arc>(primitive);
        if (std::holds_alternative<ArcUseCenterRadiusAngle>(arc)) {
            const auto &arc_ = std::get<ArcUseCenterRadiusAngle>(arc);
            return {
                    {"Arc", {
                            {"center", {{"x", arc_.center.x}, {"y", arc_.center.y}},
                             {"radius", arc_.radius},
                             {"start_angle", arc_.start_angle},
                             {"end_angle", arc_.end_angle},
                             {"color", serialize_color(arc_.options.color)}
                            }}
                    }
            };
        }
    };
    return {};
}
