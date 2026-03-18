cmake_minimum_required(VERSION 3.16)
project(gpulite VERSION 0.1.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.mm")

add_executable(gpulite main.cpp ${SOURCES})

target_include_directories(gpulite PRIVATE src)
target_compile_options(gpulite PRIVATE -Wall -Wextra)

target_link_libraries(gpulite
  "-framework Metal"
  "-framework Foundation"
  "-framework IOKit"
)

install(TARGETS gpulite DESTINATION bin)