cmake_minimum_required(VERSION 3.15)
project(yellowcat98_modify LANGUAGES C CXX RC)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_RC_COMPILER llvm-rc)

set(SOURCES
    src/main.cpp
)

add_library(yellowcat98_modify SHARED ${SOURCES})

# Rename output to .slp extension
set_target_properties(yellowcat98_modify PROPERTIES
    OUTPUT_NAME "yellowcat98.modify"
    SUFFIX ".slp"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.output"
)

CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.5.0 # wonder who taught it was a good idea to not make the actual latest version the latest version!
)

target_include_directories(${PROJECT_NAME} PRIVATE ${sol2_SOURCE_DIR}/include)

target_link_libraries(yellowcat98_modify PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/lua.lib"
)

set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/serpentlua.rc")
set(RESOURCE_OUTPUT "${CMAKE_BINARY_DIR}/serpentlua.res")

add_custom_command(
    OUTPUT ${RESOURCE_OUTPUT}
    COMMAND ${CMAKE_RC_COMPILER} /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
    DEPENDS ${RESOURCE_FILE}
    COMMENT "Compiling resource file serpentlua.rc with llvm-rc"
)

add_custom_target(serpentlua_res DEPENDS ${RESOURCE_OUTPUT})

add_dependencies(yellowcat98_modify serpentlua_res)
target_link_options(yellowcat98_modify PRIVATE ${RESOURCE_OUTPUT})

target_include_directories(yellowcat98_modify PRIVATE
    "${CMAKE_SOURCE_DIR}/lua"
)

