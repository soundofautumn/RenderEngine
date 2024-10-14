cmake_minimum_required(VERSION 3.22)
project(RenderEngineServer)

# cmake_policy(SET CMP0144 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

set(BUILD_SERVER_MODULE ON)

# Boost
find_package(Boost COMPONENTS beast json program_options CONFIG)

if (NOT Boost_FOUND)
    find_package(Boost COMPONENTS program_options)
endif()

if (NOT Boost_FOUND)
    set(BUILD_SERVER_MODULE OFF)
    message(WARNING "Boost not found")
endif ()

# spdlog
find_package(spdlog CONFIG)

if (NOT spdlog_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1  # You can set the version you want
    )
    FetchContent_MakeAvailable(spdlog)
endif()

if (NOT spdlog_FOUND AND NOT TARGET spdlog)
    set(BUILD_SERVER_MODULE OFF)
    message(WARNING "spdlog not found")
endif ()

if (BUILD_SERVER_MODULE)
    add_executable(${PROJECT_NAME} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options spdlog::spdlog RenderEngineCore)
else ()
    message(WARNING "Server module will not be built")
endif ()
