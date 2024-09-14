#include <spdlog/spdlog.h>

#include <thread>

#include "Listener.h"

int main() {
#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::debug);
#endif

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(3000);
    auto const threads = 4;

    logger::info("Server started");

    logger::info("Server listening on: {}:{}", address.to_string(), port);

    // The io_context is required for all I/O
    boost::asio::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<Listener>(ioc, tcp::endpoint{address, port})->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;

    for (auto i = threads - 1; i > 0; --i) v.emplace_back([&ioc] { ioc.run(); });

    for (auto &t : v) t.join();

    return 0;
}