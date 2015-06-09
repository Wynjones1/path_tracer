cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wno-unused-variable -Werror")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules)
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/glm/include)
find_package(SDL2)
add_subdirectory(src)
