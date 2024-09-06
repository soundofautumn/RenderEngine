cmake_minimum_required(VERSION 3.28)

project(RenderEngineCore)

set(CMAKE_CXX_STANDARD 20)

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h" "*.hpp")

add_library(RenderEngineCore STATIC ${SOURCES})

target_include_directories(RenderEngineCore PUBLIC "${source_dir}/include")

add_executable(RenderEngineCoreTest "${source_dir}/test.cpp")

target_link_libraries(RenderEngineCoreTest PRIVATE RenderEngineCore)