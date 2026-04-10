cmake_minimum_required(VERSION 3.20)

project(
    lastditch
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

find_package(glfw3 CONFIG REQUIRED)

add_executable(
    lastditch
    src/game/game.c
)

target_include_directories(
    lastditch 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/jsk
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
    external/jsk/jsk_config.c
    external/jsk/jsk_gl.c
    external/jsk/jsk_log.c      
    external/stb/src/stb_image.c
    src/core/core.c
    src/core/math/math.c
    src/game/sim/actor.c
    src/game/sim/action.c
    src/game/sim/debug.c
    src/game/sim/nation.c
    src/game/sim/navigation.c
    src/game/sim/physics.c
    src/game/sim/population.c
    src/game/sim/sim.c
    src/game/sim/world.c
    src/game/shell/render.c
    src/game/shell/screen.c
    src/game/shell/shell.c
    src/game/shell/viewpoint.c
    src/platform/platform.c
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
