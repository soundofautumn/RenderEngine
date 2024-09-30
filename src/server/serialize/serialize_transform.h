//
// Created by Autumn Sound on 2024/9/30.
//
#ifndef RENDERENGINE_SERIALIZE_TRANSFORM_H
#define RENDERENGINE_SERIALIZE_TRANSFORM_H

#include <boost/json.hpp>

#include "serialize_point.h"
#include "transform.hpp"
#include "vector.hpp"

inline RenderCore::Vector2f deserialize_vector(const boost::json::object &obj) {
    return {
        static_cast<float>(obj.at("x").as_double()), static_cast<float>(obj.at("y").as_double())};
}

inline boost::json::object serialize_vector(const RenderCore::Vector2f &vector) {
    return {{"x", vector.x}, {"y", vector.y}};
}

inline RenderCore::Translate deserialize_translate(const boost::json::object &obj) {
    RenderCore::Translate translate;
    translate.offset = deserialize_vector(obj.at("offset").as_object());
    return translate;
}

inline boost::json::object serialize_translate(const RenderCore::Translate &translate) {
    return {
        {"offset", serialize_vector(translate.offset)},
    };
}

inline RenderCore::Rotate deserialize_rotate(const boost::json::object &obj) {
    RenderCore::Rotate rotate;
    rotate.angle = static_cast<float>(obj.at("angle").as_double());
    rotate.center = deserialize_point(obj.at("center").as_object());
    return rotate;
}

inline boost::json::object serialize_rotate(const RenderCore::Rotate &rotate) {
    return {
        {"angle", rotate.angle},
        {"center", serialize_point(rotate.center)},
    };
}

inline RenderCore::Scale deserialize_scale(const boost::json::object &obj) {
    RenderCore::Scale scale;
    scale.scale_x = static_cast<float>(obj.at("scale_x").as_double());
    scale.scale_y = static_cast<float>(obj.at("scale_y").as_double());
    scale.center = deserialize_point(obj.at("center").as_object());
    return scale;
}

inline boost::json::object serialize_scale(const RenderCore::Scale &scale) {
    return {
        {"scale_x", scale.scale_x},
        {"scale_y", scale.scale_y},
        {"center", serialize_point(scale.center)},
    };
}

inline RenderCore::Transform deserialize_transform(const boost::json::object &obj) {
    if (obj.contains("Translate")) {
        return deserialize_translate(obj.at("Translate").as_object());
    } else if (obj.contains("Rotate")) {
        return deserialize_rotate(obj.at("Rotate").as_object());
    } else if (obj.contains("Scale")) {
        return deserialize_scale(obj.at("Scale").as_object());
    }
    return {};
}

inline boost::json::object serialize_transform(const RenderCore::Transform &transform) {
    if (std::holds_alternative<RenderCore::Translate>(transform)) {
        return {{"Translate", serialize_translate(std::get<RenderCore::Translate>(transform))}};
    } else if (std::holds_alternative<RenderCore::Rotate>(transform)) {
        return {{"Rotate", serialize_rotate(std::get<RenderCore::Rotate>(transform))}};
    } else if (std::holds_alternative<RenderCore::Scale>(transform)) {
        return {{"Scale", serialize_scale(std::get<RenderCore::Scale>(transform))}};
    }
    return {};
}

#endif
