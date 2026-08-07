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
	NAME luaJIT
	GITHUB_REPOSITORY LuaJIT/LuaJIT
	GIT_TAG 1edc3e52b67eaf6ce5f809be8e17d6862594b8bc
	DOWNLOAD_ONLY TRUE
)

set(LUAJIT_DIR "${luaJIT_SOURCE_DIR}")
CPMAddPackage(
    NAME luajit-cmake
    GITHUB_REPOSITORY zhaozg/luajit-cmake
    GIT_TAG bdc1b54d0572e2cc5c3eaa5c1e818d0e0246b9d1
	OPTIONS
		"CMAKE_C_COMPILER clang"
		"CMAKE_CXX_COMPILER clang++"
)

CPMAddPackage(
	NAME sol2
	GITHUB_REPOSITORY ThePhD/sol2
	VERSION 3.5.0 # wonder who taught it was a good idea to not make the actual latest version the latest version!
)
target_include_directories(${PROJECT_NAME} PRIVATE ${sol2_SOURCE_DIR}/include)

CPMAddPackage(
	NAME argon
	GITHUB_REPOSITORY GlobedGD/argon
	VERSION 1.4.9
)
target_link_libraries(${PROJECT_NAME} argon)

add_dependencies(${PROJECT_NAME} libluajit)
target_include_directories(${PROJECT_NAME} PRIVATE ${luajit-cmake_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} luajit::lib luajit::header)

setup_geode_mod(${PROJECT_NAME})
