cmake_minimum_required(VERSION 3.20)

project(
    lastditch
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)

add_executable(
    lastditch
    src/main.c
)

target_include_directories(
    lastditch 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/jsk
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
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
        "-framework OpenGL"
    )
endif()

target_sources(
    lastditch 
    PRIVATE
    external/stb/src/stb_image.c
    external/glad/src/glad.c
    external/jsk/jsk_log.c
    external/jsk/jsk_config.c
    external/jsk/jsk_gl.c
    src/camera.c
    src/world.c
    src/input.c
    src/render.c
    src/screen.c
)

target_compile_definitions(
    lastditch
    PRIVATE
    GLFW_INCLUDE_NONE
    GL_SILENCE_DEPRECATION
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
