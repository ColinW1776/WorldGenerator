cmake_minimum_required(VERSION 3.23)
project(Continents C)

set(CMAKE_C_STANDARD 99)

add_executable(Continents main.c)

set_source_files_properties(main.c PROPERTIES COMPILE_OPTIONS "-O3")