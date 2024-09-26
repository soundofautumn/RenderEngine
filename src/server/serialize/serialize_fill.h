//
// Created by Autumn Sound on 2024/9/23.
//
#ifndef RENDERENGINE_SERIALIZE_FILL_H
#define RENDERENGINE_SERIALIZE_FILL_H

#include <boost/json.hpp>

#include "fill.hpp"
#include "serialize_point.h"

inline boost::json::object serialize_fill(const RenderCore::Fill &fill) {
    return {{"seed", serialize_point(fill.seed)}};
}

inline RenderCore::Fill deserialize_fill(const boost::json::object &obj) {
    return RenderCore::Fill{.seed = deserialize_point(obj.at("seed").as_object())};
}
#endif  //RENDERENGINE_SERIALIZE_FILL_H
