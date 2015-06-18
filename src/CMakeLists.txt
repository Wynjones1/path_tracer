cmake_minimum_required(VERSION 2.8)

file(GLOB SOURCES FILES *.cpp)
file(GLOB HEADERS FILES *.h)
file(GLOB_RECURSE PLY FILES ${CMAKE_SOURCE_DIR}/data/*.ply)
file(GLOB_RECURSE SCENES FILES ${CMAKE_SOURCE_DIR}/data/scenes/*.scene)

set(CONFIG_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
make_directory(${CONFIG_INCLUDE_DIR})
configure_file(config.i ${CONFIG_INCLUDE_DIR}/config.h)

include_directories(
    ${CONFIG_INCLUDE_DIR}
)

include_directories( SYSTEM
    ${SDL2_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
	${RAPIDJSON_INCLUDE_DIR}
)
set_source_files_properties(${PLY} PROPERTIES HEADER_FILE_ONLY True)
set_source_files_properties(${SCENES} PROPERTIES HEADER_FILE_ONLY True)
source_group(Scenes FILES ${SCENES})
source_group(PLY FILES ${PLY})
add_executable(path_tracer ${SOURCES} ${HEADERS} ${PLY} ${SCENES})
target_link_libraries(path_tracer ${SDL2_LIBRARY})

add_custom_command(TARGET     path_tracer
                   POST_BUILD
                   COMMAND    ${CMAKE_COMMAND} -E copy_if_different ${SDL2_DLL} $<TARGET_FILE_DIR:path_tracer>)
