cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(SerpentLua VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/internal/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "include/internal")

if(PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE YELLOWCAT98_SERPENTLUA_EXPORTING)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage(
    NAME lua
    GITHUB_REPOSITORY lua/lua
    VERSION 5.4.6
    DOWNLOAD_ONLY TRUE
)

if(PROJECT_IS_TOP_LEVEL) # sol is purely used to serpentlua's internal plugin
CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.3.0
)

target_include_directories(${PROJECT_NAME} PRIVATE ${sol2_SOURCE_DIR}/include)
endif()

# build lua both statically and dynmamically so you can link against it

file(GLOB LUA_SRC ${lua_SOURCE_DIR}/*.c)
list(REMOVE_ITEM LUA_SRC
    ${lua_SOURCE_DIR}/onelua.c
    ${lua_SOURCE_DIR}/lua.c # this is the interpreter cli itself, not needed
)

# this is the lua interpreter for serpent itself, serpentlua statically links against this to prevent issues with requiring to have lua installed in PATH.
add_library(lua_for_serpent STATIC ${LUA_SRC})
target_include_directories(lua_for_serpent PUBLIC ${lua_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} lua_for_serpent)

# this is the lua for plugins, this is the lua that plugins dynamically link against, dynamically linking is better than static linking because it allows to use one lua runtime for all plugins.
add_library(lua SHARED ${LUA_SRC})
target_include_directories(lua PRIVATE include)
target_compile_definitions(lua PRIVATE LUA_BUILD_AS_DLL)

setup_geode_mod(${PROJECT_NAME})
