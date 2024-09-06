cmake_minimum_required(VERSION 3.28)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_subdirectory(src/core)
add_subdirectory(src/server)