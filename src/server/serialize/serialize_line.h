//
// Created by Autumn Sound on 2024/9/18.
//
#ifndef RENDERENGINE_SERIALIZE_LINE_H
#define RENDERENGINE_SERIALIZE_LINE_H

#include <boost/json.hpp>

#include "line.hpp"
#include "serialize_options.h"
#include "serialize_point.h"

boost::json::object serialize_line(const RenderCore::Line &line) {
    return {{"p1", serialize_point(line.p1)}, {"p2", serialize_point(line.p2)},
        {"algorithm", static_cast<int64_t>(line.algorithm)}};
}

RenderCore::Line deserialize_line(const boost::json::object &obj) {
    return RenderCore::Line{.p1 = deserialize_point(obj.at("p1").as_object()),
        .p2 = deserialize_point(obj.at("p2").as_object()),
        .algorithm = static_cast<RenderCore::Line::LineAlgorithm>(obj.at("algorithm").as_int64())};
}

#endif
