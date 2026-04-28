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

find_package(glfw3 CONFIG REQUIRED)

add_executable(
    lastditch
    src/game/game.cpp
)

target_include_directories(
    lastditch 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/justsky
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    lastditch 
    PRIVATE
    glfw
)

if(APPLE)
	target_link_libraries(
		lastditch
		PRIVATE
        glfw
		${OPENGL_FRAMEWORK}
		${COCOA_FRAMEWORK}
		${IOKIT_FRAMEWORK}
		${COREVIDEO_FRAMEWORK}
	)
endif()

target_sources(
    lastditch
    PRIVATE
    external/glad/src/glad.c
    external/stb/src/stb_image.c
    external/justsky/justsky_log.cpp
    external/justsky/justsky_config.cpp
    src/core/math_ext.cpp
    src/game/sim/actor.cpp
    src/game/sim/action.cpp
    src/game/sim/area.cpp
    src/game/sim/debug.cpp
    src/game/sim/direction.cpp
    src/game/sim/nation.cpp
    src/game/sim/navigation.cpp
    src/game/sim/physics.cpp
    src/game/sim/population.cpp
    src/game/sim/work.cpp
    src/game/sim/sim.cpp
    src/game/sim/world.cpp
    src/game/shell/gl_ext.cpp
    src/game/shell/render.cpp
    src/game/shell/screen.cpp
    src/game/shell/shell.cpp
    src/game/shell/viewpoint.cpp
    src/platform/platform.cpp
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
    COMMAND ${CMAKE_COMMAND} -E echo "Assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
	        $<TARGET_FILE_DIR:lastditch>/assets
)
