//
// Created by Autumn Sound on 2024/9/12.
//

#ifndef RENDERENGINE_SERIALIZE_H
#define RENDERENGINE_SERIALIZE_H

#include <boost/json.hpp>
#include "primitive.hpp"

RenderCore::Primitive deserialize_primitive(const boost::json::object &obj);

boost::json::object serialize_primitive(const RenderCore::Primitive &primitive);

#endif //RENDERENGINE_SERIALIZE_H
