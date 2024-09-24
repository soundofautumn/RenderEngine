cmake_minimum_required(VERSION 3.22)
project(RenderEngineServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

set(BUILD_SERVER_MODULE ON)

# Boost.Beast
find_package(Boost COMPONENTS beast json CONFIG)

if (NOT Boost_FOUND)
    set(BUILD_SERVER_MODULE OFF)
    message(WARNING "Boost not found")
endif ()

# spdlog
find_package(spdlog CONFIG)

if (NOT spdlog_FOUND)
    set(BUILD_SERVER_MODULE OFF)
    message(WARNING "spdlog not found")
endif ()

if (BUILD_SERVER_MODULE)
    add_executable(${PROJECT_NAME} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::beast Boost::json spdlog::spdlog RenderEngineCore)
else ()
    message(WARNING "Server module will not be built")
endif ()
