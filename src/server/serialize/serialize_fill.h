//
// Created by Autumn Sound on 2024/9/23.
//
#ifndef RENDERENGINE_SERIALIZE_FILL_H
#define RENDERENGINE_SERIALIZE_FILL_H

#include <boost/json.hpp>

#include "fill.hpp"
#include "serialize_point.h"

using namespace RenderCore;

boost::json::object serialize_fill(const Fill &fill) {
    return {{"seed", serialize_point(fill.seed)}};
}

Fill deserialize_fill(const boost::json::object &obj) {
    return Fill{.seed = deserialize_point(obj.at("seed").as_object())};
}
#endif  //RENDERENGINE_SERIALIZE_FILL_H
