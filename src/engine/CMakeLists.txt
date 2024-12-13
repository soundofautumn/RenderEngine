cmake_minimum_required(VERSION 3.22)

project(RenderEngineCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Build type not specified: Defaulting to Debug")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(RENDERENGINE_DEBUG)
endif()

file(GLOB SOURCES "${source_dir}/core/*.cpp")
file(GLOB HEADERS "${source_dir}/include/*.h" "${source_dir}/include/*.hpp")

add_library(RenderEngineCore STATIC ${SOURCES})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(RenderEngineCore PRIVATE -O3 -flto -fno-exceptions -fno-rtti -march=native)
        target_link_options(RenderEngineCore PRIVATE -flto -Wl,--as-needed -Wl,-O3)
    elseif (MSVC)
        target_compile_options(RenderEngineCore PRIVATE /O2 /GL /DNDEBUG)
        target_link_options(RenderEngineCore PRIVATE /LTCG)
    endif ()
endif ()

target_include_directories(RenderEngineCore PUBLIC "${source_dir}/include")

add_executable(RenderEngineCoreTest "${source_dir}/test.cpp")

target_link_libraries(RenderEngineCoreTest PRIVATE RenderEngineCore)
