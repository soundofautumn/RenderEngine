//
// Created by Autumn Sound on 2024/9/12.
//

#ifndef RENDERENGINE_ENGINEMANGER_H
#define RENDERENGINE_ENGINEMANGER_H

#include <memory>
#include <unordered_map>
#include <string>

#include "engine.hpp"

using RenderCore::RenderEngine;

class EngineManager {
    std::unordered_map<std::string, std::shared_ptr<RenderEngine>> engines_;
    std::mutex mutex_;

public:

    void create_engine(const std::string &name, int width, int height) {
        std::lock_guard<std::mutex> lock(mutex_);
        engines_[name] = std::make_shared<RenderEngine>(width, height);
    }

    std::shared_ptr<RenderEngine> get_engine(const std::string &name) {
        return engines_[name];
    }

    void remove_engine(const std::string &name) {
        std::lock_guard<std::mutex> lock(mutex_);
        engines_.erase(name);
    }
};

#endif //RENDERENGINE_ENGINEMANGER_H
