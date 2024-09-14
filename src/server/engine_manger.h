//
// Created by Autumn Sound on 2024/9/12.
//

#ifndef RENDERENGINE_ENGINEMANGER_H
#define RENDERENGINE_ENGINEMANGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "engine.hpp"

using RenderCore::RenderEngine;

class EngineManager {
   public:
    struct EngineMutex {
        RenderEngine engine;
        std::mutex mutex;

        EngineMutex() = default;

        EngineMutex(int width, int height) : engine(width, height) {}
    };

   private:
    std::unordered_map<std::string, std::shared_ptr<EngineMutex>> engines_;
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
        engines_[name] = std::make_shared<EngineMutex>(width, height);
    }

    bool check_engine(const std::string &name) { return engines_.find(name) != engines_.end(); }

    std::shared_ptr<EngineMutex> get_engine_with_mutex(const std::string &name) {
        return engines_[name];
    }

    void remove_engine(const std::string &name) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        logger::info("Remove engine: {}", name);
        engines_.erase(name);
    }
};

#endif  //RENDERENGINE_ENGINEMANGER_H
