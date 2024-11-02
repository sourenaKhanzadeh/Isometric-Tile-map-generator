cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/ADMIN/gitHUB/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Fortifier LANGUAGES CXX)

include(cmake/include.cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
include(cmake/CPM.cmake)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs)

# FetchContent_MakeAvailable(mapgen)

# Gather all subdirectories in src and create libraries from each
file(GLOB SUBDIRECTORIES RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/*")
set(LIBRARIES_TO_LINK)

find_path(MAPBOX_GEOMETRY_INCLUDE_DIRS "mapbox/feature.hpp")
find_path(MAPBOX_VARIANT_INCLUDE_DIRS "mapbox/optional.hpp")

set(LIB_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/")


fetch_and_create_library(MapRenderer src/Map nlohmann_json sfml-graphics)
fetch_and_create_library(CameraController src/Camera sfml-graphics)
fetch_and_create_library(Utils src/Utils  nlohmann_json sfml-graphics)

list(APPEND LIBRARIES_TO_LINK MapRenderer CameraController Utils)

target_include_directories(MapRenderer PRIVATE ${LIB_INCLUDE_DIRS})

# Main executable
add_executable(main src/main.cpp)
# Link all libraries created from subdirectories and additional dependencies
target_link_libraries(main PRIVATE ${LIBRARIES_TO_LINK} sfml-graphics nlohmann_json ImGui-SFML::ImGui-SFML)
target_include_directories(main PRIVATE ${MAPBOX_GEOMETRY_INCLUDE_DIRS} ${MAPBOX_VARIANT_INCLUDE_DIRS} ${LIB_INCLUDE_DIRS})

target_compile_features(main PRIVATE cxx_std_17)


if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
