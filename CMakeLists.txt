cmake_minimum_required(VERSION 2.8)

if(MSVC)
	add_definitions(/D_CRT_SECURE_NO_WARNINGS)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wno-unused-variable -Werror")
endif()

set(CMAKE_BUILD_TYPE  Debug)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules)
set(GLM_INCLUDE_DIR   ${CMAKE_SOURCE_DIR}/deps/glm/include)
set(RAPIDJSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/rapidjson/include)

if(MSVC)
	set(WIN_ARCH x64)
	set(WIN_ARCH x86)
	set(SDL2_DIR     ${CMAKE_SOURCE_DIR}/deps/sdl/windows)
	set(SDL2_LIB_DIR ${SDL2_DIR}/lib/${WIN_ARCH})
    set(SDL2_DLL ${SDL2_LIB_DIR}/SDL2.dll)
	set(SDL2MAIN_LIBRARY ${SDL2_LIB_DIR}/SDL2main.lib)
	set(SDL2_INCLUDE_DIR ${SDL2_DIR}/include)
	
	set(SDL2_LIBRARY
			${SDL2_LIB_DIR}/SDL2main.lib
			${SDL2_LIB_DIR}/SDL2.lib
		)
else()
	find_package(SDL2)
endif()
add_subdirectory(src)
