cmake_minimum_required(VERSION 3.28)

project(RenderEngineCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB SOURCES "${source_dir}/core/*.cpp")
file(GLOB HEADERS "${source_dir}/include/*.h" "${source_dir}/include/*.hpp")

add_library(RenderEngineCore STATIC ${SOURCES})

target_include_directories(RenderEngineCore PUBLIC "${source_dir}/include")

add_executable(RenderEngineCoreTest "${source_dir}/test.cpp")

target_link_libraries(RenderEngineCoreTest PRIVATE RenderEngineCore)
