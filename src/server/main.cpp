#include <boost/program_options.hpp>
#include <iostream>
#include <thread>

#include "Listener.h"
#include "EngineManager.h"
#include "logger.h"

namespace po = boost::program_options;
namespace logger = spdlog;

int main(int argc, char *argv[]) {
    // 定义一个描述命令行参数的对象
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")(
        "port,p", po::value<unsigned short>()->default_value(3000), "set port number")(
        "address,a", po::value<std::string>()->default_value("0.0.0.0"), "set server address")(
        "threads,t", po::value<int>()->default_value(4), "set number of threads");

    // 存储命令行参数的变量
    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);  // 通知并解析参数
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // 处理 --help 选项
    if (vm.count("help")) {
        desc.print(std::cout);
        return 0;
    }

    // 获取参数值
    auto const address = boost::asio::ip::make_address(vm["address"].as<std::string>());
    auto const port = vm["port"].as<unsigned short>();
    auto const threads = vm["threads"].as<int>();

    init_logger();

    logger::info("Server started");

    logger::info("Server listening on: {}:{}", address.to_string(), port);

    // The io_context is required for all I/O
    boost::asio::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<Listener>(ioc, tcp::endpoint{address, port})->run();

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](boost::system::error_code const &, int signal_number) {
        spdlog::info("Received signal {}, shutting down...", signal_number);
        EngineManager::get_instance().shutdown();
        ioc.stop();  // 停止 io_context
    });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;

    for (auto i = threads - 1; i > 0; --i) v.emplace_back([&ioc] { ioc.run(); });

    ioc.run();

    for (auto &t : v) t.join();

    spdlog::shutdown();

    return 0;
}
