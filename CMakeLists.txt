cmake_minimum_required(VERSION 3.20)

project(
    lastditch
    VERSION 0.1
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
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
    src/lastditch.cpp
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
    external/glad/src/gl.c
    external/stb/src/stb_image.c
    src/app/action.cpp
    src/app/action.h
    src/app/actor.cpp
    src/app/actor.h
    src/app/app.h
    src/app/app.cpp
    src/app/area.cpp
    src/app/area.h
    src/app/debug.cpp
    src/app/debug.h
    src/app/direction.cpp
    src/app/direction.h
    src/app/nation.cpp
    src/app/nation.h
    src/app/navigation.cpp
    src/app/navigation.h
    src/app/physics.cpp
    src/app/physics.h
    src/app/population.cpp
    src/app/population.h
    src/app/render.cpp
    src/app/render.h
    src/app/screen.cpp
    src/app/screen.h
    src/app/state.h
    src/app/state.cpp
    src/app/viewpoint.h
    src/app/work.cpp
    src/app/work.h
    src/app/world.cpp
    src/app/world.h
    src/core/config.h
    src/core/config.cpp
    src/core/geometry.h
    src/core/geometry.cpp
    src/core/log.h
    src/core/log.cpp
    src/core/random.cpp
    src/core/random.h
    src/core/macros.h
    src/core/types.h
    src/platform/platform.cpp
    src/platform/platform.h
    src/platform/window.cpp
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
    -Wno-missing-braces
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
