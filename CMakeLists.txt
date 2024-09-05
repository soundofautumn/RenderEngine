cmake_minimum_required(VERSION 3.28)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 17)

# 自动查找所有 .cpp 和 .h 文件
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${source_dir}/*.cpp" "${source_dir}/*.h")
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")

find_package(ZLIB REQUIRED)
find_library(USOCKETS_LIBRARY uSockets.a PATHS /usr/local/lib REQUIRED)

add_executable(RenderEngine ${SOURCES})

target_include_directories(RenderEngine PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
target_link_libraries(RenderEngine PRIVATE ZLIB::ZLIB ${USOCKETS_LIBRARY})