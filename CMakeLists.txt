cmake_minimum_required(VERSION 3.10)
project(SubgraphMatching)
set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE MAIN_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(run ${MAIN_SRC})