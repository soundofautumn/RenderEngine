cmake_minimum_required(VERSION 3.22)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/utf-8)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -pedantic)
    add_compile_options(-Wno-gnu-anonymous-struct -Wno-nested-anon-types)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_subdirectory(src/engine)
add_subdirectory(src/server)
