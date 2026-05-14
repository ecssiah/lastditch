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
    src/lastditch.cc
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
    src/app/render.cc
    src/app/render.h
    src/app/screen.cc
    src/app/screen.h
    src/app/shell.cc
    src/app/shell.h
    src/app/viewpoint.cc
    src/app/viewpoint.h
    src/app/actor.cc
    src/app/actor.h
    src/app/action.cc
    src/app/action.h
    src/app/area.cc
    src/app/area.h
    src/app/debug.cc
    src/app/debug.h
    src/app/direction.cc
    src/app/direction.h
    src/app/nation.cc
    src/app/nation.h
    src/app/navigation.cc
    src/app/navigation.h
    src/app/physics.cc
    src/app/physics.h
    src/app/population.cc
    src/app/population.h
    src/app/work.cc
    src/app/work.h
    src/app/sim.cc
    src/app/sim.h
    src/app/world.cc
    src/app/world.h
    src/platform/input.cc
    src/platform/input.h
    src/platform/platform.cc
    src/platform/platform.h
    src/platform/window.cc
    src/platform/window.h        
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
