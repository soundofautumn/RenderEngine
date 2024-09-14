cmake_minimum_required(VERSION 3.28)
project(RenderEngine)

if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_subdirectory(src/engine)
add_subdirectory(src/server)
