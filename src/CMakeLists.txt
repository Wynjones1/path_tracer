cmake_minimum_required(VERSION 2.8)

file(GLOB SOURCES FILES *.cpp)
file(GLOB HEADERS FILES *.h)

set(CONFIG_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
make_directory(${CONFIG_INCLUDE_DIR})
configure_file(config.i ${CONFIG_INCLUDE_DIR}/config.h)

include_directories(
    ${CONFIG_INCLUDE_DIR}
)

include_directories( SYSTEM
    ${SDL2_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)
add_executable(path_tracer ${SOURCES} ${HEADERS})
target_link_libraries(path_tracer ${SDL2_LIBRARY})
