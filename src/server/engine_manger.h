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
    const std::chrono::seconds engine_timeout = std::chrono::seconds(30);

    std::unordered_map<std::string, std::shared_ptr<EngineMutex>> engines_;
    std::unordered_map<std::string, boost::asio::steady_timer> timers_;
    std::mutex map_mutex_;

    const int timer_thread_count = 4;
    std::vector<std::thread> threads_;
    boost::asio::io_context ioc_{timer_thread_count};
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_{ioc_.get_executor()};

    void reset_timer(const std::string &name) {
        if (timers_.find(name) == timers_.end()) {
            timers_.emplace(name, boost::asio::steady_timer(ioc_));
        }

        timers_.at(name).expires_after(engine_timeout);
        timers_.at(name).async_wait([this, name](const boost::system::error_code &ec) {
            if (!ec) {
                logger::info("Engine {} timeout", name);
                remove_engine(name);
            }
        });
    }

   public:
    EngineManager() {
        for (int i = 0; i < timer_thread_count; i++) {
            threads_.emplace_back([this] { ioc_.run(); });
        }
    }

    static EngineManager &get_instance() {
        static EngineManager instance;
        return instance;
    }

    void create_engine(const std::string &name, int width, int height) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        logger::info("Create engine: {}", name);
        engines_[name] = std::make_shared<EngineMutex>(width, height);
        reset_timer(name);
    }

    bool check_engine(const std::string &name) { return engines_.find(name) != engines_.end(); }

    std::shared_ptr<EngineMutex> get_engine_with_mutex(const std::string &name) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        if (engines_.find(name) == engines_.end()) {
            return nullptr;
        }
        reset_timer(name);
        return engines_[name];
    }

    void remove_engine(const std::string &name) {
        std::lock_guard<std::mutex> lock(map_mutex_);
        logger::info("Remove engine: {}", name);
        engines_.erase(name);
    }
};

#endif  //RENDERENGINE_ENGINEMANGER_H
