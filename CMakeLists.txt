cmake_minimum_required(VERSION 3.28)
project(RenderEngine)

if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/utf-8)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
    add_compile_options(-Wno-gnu-anonymous-struct -Wno-nested-anon-types)
endif ()

add_subdirectory(src/engine)
add_subdirectory(src/server)
