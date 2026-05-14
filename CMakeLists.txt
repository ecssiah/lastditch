cmake_minimum_required(VERSION 3.20)

project(
    lastditch
    VERSION 0.1
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.4)

add_executable(
    lastditch
    src/game/lastditch.cc
)

target_include_directories(
    lastditch
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    lastditch
    PRIVATE
    glfw
)

if (APPLE)
    target_link_libraries(
        lastditch
        PRIVATE
        ${OPENGL_FRAMEWORK}
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
endif ()

target_sources(
    lastditch
    PRIVATE
    external/glad/src/glad.c
    external/stb/src/stb_image.c
    src/core/config.h
    src/core/config.cc
    src/core/geometry.h
    src/core/geometry.cc
    src/core/log.h
    src/core/log.cc
    src/core/macros.h
    src/core/types.h
    src/game/shell/render.cc
    src/game/shell/render.h
    src/game/shell/screen.cc
    src/game/shell/screen.h
    src/game/shell/shell.cc
    src/game/shell/shell.h
    src/game/shell/viewpoint.cc
    src/game/shell/viewpoint.h
    src/game/sim/actor.cc
    src/game/sim/actor.h
    src/game/sim/action.cc
    src/game/sim/action.h
    src/game/sim/area.cc
    src/game/sim/area.h
    src/game/sim/debug.cc
    src/game/sim/debug.h
    src/game/sim/direction.cc
    src/game/sim/direction.h
    src/game/sim/ids.h
    src/game/sim/nation.cc
    src/game/sim/nation.h
    src/game/sim/navigation.cc
    src/game/sim/navigation.h
    src/game/sim/physics.cc
    src/game/sim/physics.h
    src/game/sim/population.cc
    src/game/sim/population.h
    src/game/sim/work.cc
    src/game/sim/work.h
    src/game/sim/sim.cc
    src/game/sim/sim.h
    src/game/sim/world.cc
    src/game/sim/world.h
    src/platform/platform.cc
    src/platform/platform.h
)

target_compile_definitions(
    lastditch
    PRIVATE
    GLFW_INCLUDE_NONE
    GL_SILENCE_DEPRECATION
)

target_compile_options(
    lastditch
    PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
)

add_custom_command(
    TARGET
    lastditch
    POST_BUILD
    COMMAND 
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:lastditch>/assets
    COMMAND 
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:lastditch>/config
)
