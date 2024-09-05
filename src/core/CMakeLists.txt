cmake_minimum_required(VERSION 3.28)

project(RenderEngineCore)

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${source_dir}/*.cpp" "${source_dir}/*.h")

add_library(RenderEngineCore STATIC ${SOURCES})

target_include_directories(RenderEngineCore PUBLIC "${source_dir}")

add_executable(RenderEngineCoreTest "${source_dir}/test.cpp")