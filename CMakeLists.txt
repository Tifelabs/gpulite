cmake_minimum_required(VERSION 3.16)
project(gpulite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(gpulite main.cpp ${SOURCES})

target_include_directories(gpulite PRIVATE src)
target_compile_options(gpulite PRIVATE -Wall -Wextra)

install(TARGETS gpulite DESTINATION bin)