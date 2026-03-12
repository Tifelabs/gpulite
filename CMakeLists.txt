cmake_minimum_required(VERSION 3.16)
project(gpulite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(gpulite src/main.cpp)

target_compile_options(gpulite PRIVATE -Wall -Wextra)

install(TARGETS gpulite DESTINATION bin)