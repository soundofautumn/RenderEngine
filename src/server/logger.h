//
// Created by Autumn Sound on 2024/9/19.
//
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


void init_logger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef NDEBUG
    console_sink->set_level(spdlog::level::info);
#else
    console_sink->set_level(spdlog::level::debug);
    // 刷新缓冲区
    spdlog::flush_on(spdlog::level::trace);
#endif
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logs/server.log", 1024 * 1024 * 5, 3);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    auto combined_logger = std::make_shared<spdlog::logger>(
        "server", spdlog::sinks_init_list{console_sink, file_sink});
    spdlog::set_default_logger(combined_logger);
    spdlog::set_level(spdlog::level::trace);
}
