# README:
# This is the CMake configuration file for this game project.
# See the file build.bat or build.sh for info on how to build with CMake.

cmake_minimum_required(VERSION 3.11)

# Project settings
# -----------------------------------------------------------------------------
project(FroggerRemake C) # name of project files
set(OUTPUT_NAME frogger) # executable name
if (PLATFORM STREQUAL "Web")
    set(OUTPUT_NAME index) # e.g. index.html
endif()
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generate compile_commands.json
set(CMAKE_VERBOSE_MAKEFILE ON) # show build commands

# Download and build raylib if needed
# -----------------------------------------------------------------------------
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
  endif()
endif()

# Define target
# -----------------------------------------------------------------------------
add_executable(${OUTPUT_NAME})
target_sources(${OUTPUT_NAME} PRIVATE             src/main.c)
target_include_directories(${OUTPUT_NAME} PRIVATE deps)
target_link_libraries(${OUTPUT_NAME} PRIVATE      raylib)

# Platform settings
# -----------------------------------------------------------------------------
if(PLATFORM STREQUAL "Web")
    set_target_properties(${OUTPUT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${OUTPUT_NAME} PRIVATE
        --shell-file ${CMAKE_SOURCE_DIR}/shell.html --preload-file ${CMAKE_SOURCE_DIR}/assets@assets
        -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sTOTAL_MEMORY=67108864
        -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sEXPORTED_RUNTIME_METHODS=HEAPF32)
elseif (UNIX)
    target_link_libraries(${OUTPUT_NAME} PRIVATE m)
elseif (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${OUTPUT_NAME})
    set_target_properties(${OUTPUT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
elseif (APPLE)
    target_link_libraries(${OUTPUT_NAME} "-framework IOKit")
    target_link_libraries(${OUTPUT_NAME} "-framework Cocoa")
    target_link_libraries(${OUTPUT_NAME} "-framework OpenGL")
endif()
