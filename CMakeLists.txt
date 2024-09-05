cmake_minimum_required(VERSION 3.28)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 17)

# 自动查找所有 .cpp 和 .h 文件
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${source_dir}/*.cpp" "${source_dir}/*.h")

# Boost.Beast
find_package(Boost REQUIRED COMPONENTS beast CONFIG)
# spdlog
find_package(spdlog CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::beast spdlog::spdlog)
