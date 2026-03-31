cmake_minimum_required(VERSION 3.20)

project(
    lastditch
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

find_package(glfw3 CONFIG REQUIRED)

###########
# LD Game #
###########

add_executable(
    ld_game
    src/game/game.c
)

target_include_directories(
    ld_game 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/jsk
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    ld_game 
    PRIVATE
    glfw
)

if(APPLE)
	target_link_libraries(
		ld_game
		PRIVATE
        glfw
		${OPENGL_FRAMEWORK}
		${COCOA_FRAMEWORK}
		${IOKIT_FRAMEWORK}
		${COREVIDEO_FRAMEWORK}
	)
endif()

target_sources(
    ld_game 
    PRIVATE
    external/glad/src/glad.c
    external/jsk/jsk_config.c
    external/jsk/jsk_gl.c
    external/jsk/jsk_log.c      
    external/stb/src/stb_image.c
    src/core/action.c
    src/game/sim/actor.c
    src/game/sim/physics.c
    src/game/sim/sim.c
    src/game/sim/world.c
    src/game/shell/render.c
    src/game/shell/screen.c
    src/game/shell/shell.c
    src/game/shell/viewpoint.c
    src/platform/platform.c
)

target_compile_definitions(
    ld_game
    PRIVATE
    GLFW_INCLUDE_NONE
    GL_SILENCE_DEPRECATION
)

add_custom_command(
    TARGET
    ld_game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
	        $<TARGET_FILE_DIR:ld_game>/assets
)

#############
# LD Editor #
#############

add_executable(
    ld_edit
    src/edit/edit.c
)

target_include_directories(
    ld_edit
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/jsk
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    ld_edit
    PRIVATE
    glfw
)

if(APPLE)
	target_link_libraries(
		ld_edit
		PRIVATE
        glfw
		${OPENGL_FRAMEWORK}
		${COCOA_FRAMEWORK}
		${IOKIT_FRAMEWORK}
		${COREVIDEO_FRAMEWORK}
	)
endif()

target_sources(
    ld_edit
    PRIVATE
    external/glad/src/glad.c
    external/jsk/jsk_config.c
    external/jsk/jsk_gl.c
    external/jsk/jsk_log.c      
    external/stb/src/stb_image.c
    src/core/action.c
    src/platform/platform.c
)

target_compile_definitions(
    ld_edit
    PRIVATE
    GLFW_INCLUDE_NONE
    GL_SILENCE_DEPRECATION
)

add_custom_command(
    TARGET
    ld_edit
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
	        $<TARGET_FILE_DIR:ld_edit>/assets
)

