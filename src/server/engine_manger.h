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
    std::unordered_map<std::string, std::mutex> engine_mutex_;
    std::mutex map_mutex_;

public:

    EngineManager() = default;

    static EngineManager &get_instance() {
        static EngineManager instance;
        return instance;
    }

    void create_engine(const std::string &name, int width, int height) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        logger::info("Create engine: {}", name);
        engines_[name] = std::make_shared<RenderEngine>(width, height);
    }

    std::shared_ptr<RenderEngine> get_engine(const std::string &name) {
        return engines_[name];
    }

    bool check_engine(const std::string &name) {
        return engines_.find(name) != engines_.end();
    }

    std::mutex &get_engine_mutex(const std::string &name) {
        return engine_mutex_[name];
    }

    void remove_engine(const std::string &name) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        logger::info("Remove engine: {}", name);
        engines_.erase(name);
    }
};

#endif //RENDERENGINE_ENGINEMANGER_H
