cmake_minimum_required(VERSION 3.10)
project(compression_cpp)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

include_directories(include)
add_subdirectory(submodules/googletest)
add_subdirectory(test)

add_executable(compress src/compress.cpp)