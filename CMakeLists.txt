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

if(APPLE)
        enable_language(OBJC)
        enable_language(OBJCXX)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_OPENGL OFF CACHE BOOL "" FORCE)
set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)

set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3-3.4.14
        EXCLUDE_FROM_ALL
)

add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3_ttf-3.2.2
        EXCLUDE_FROM_ALL
)

if(WIN32)
        set(
                PROJECT_PYTHON_EXECUTABLE
                "${CMAKE_CURRENT_SOURCE_DIR}/.venv/Scripts/python.exe"
        )
else()
        set(
                PROJECT_PYTHON_EXECUTABLE
                "${CMAKE_CURRENT_SOURCE_DIR}/.venv/bin/python"
        )
endif()

if(NOT EXISTS "${PROJECT_PYTHON_EXECUTABLE}")
        message(FATAL_ERROR
                "The project Python environment does not exist.\n"
                "Run tools/bootstrap.sh before configuring the project."
        )
endif()

set(
        Python3_EXECUTABLE
        "${PROJECT_PYTHON_EXECUTABLE}"
        CACHE FILEPATH
        "Python interpreter used for code generation"
        FORCE
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(BLOCK_DATA_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/data/block_data.yml")
set(BLOCK_DATA_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_block_data.py")
set(BLOCK_DATA_GENERATED_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(BLOCK_DATA_GENERATED_HEADER "${BLOCK_DATA_GENERATED_DIRECTORY}/block_data.generated.h")

add_custom_command(
        OUTPUT "${BLOCK_DATA_GENERATED_HEADER}"

        COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${BLOCK_DATA_GENERATED_DIRECTORY}"

        COMMAND
        "${Python3_EXECUTABLE}"
        "${BLOCK_DATA_GENERATOR}"
        "${BLOCK_DATA_CONFIG}"
        "${BLOCK_DATA_GENERATED_HEADER}"

        DEPENDS
        "${BLOCK_DATA_GENERATOR}"
        "${BLOCK_DATA_CONFIG}"

        VERBATIM
)

add_executable(lastditch src/lastditch.cpp)

add_custom_target(
        generate_block_types
        DEPENDS "${BLOCK_DATA_GENERATED_HEADER}"
)

add_dependencies(lastditch generate_block_types)

target_link_libraries(
        lastditch
        PRIVATE
        SDL3_ttf::SDL3_ttf
        SDL3::SDL3
)

target_include_directories(
        lastditch
        PRIVATE
        "${BLOCK_DATA_GENERATED_DIRECTORY}"
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(
        lastditch
        PRIVATE
        "${BLOCK_DATA_GENERATED_HEADER}"
        src/app/actor.h
        src/app/app.cpp
        src/app/app.h
        src/app/area.h
        src/app/cell.h
        src/app/control.cpp
        src/app/control.h
        src/app/debug.cpp
        src/app/debug.h
        src/app/direction.h
        src/app/nation.h
        src/app/navigation.h
        src/app/navigation.cpp
        src/app/physics.cpp
        src/app/physics.h
        src/app/population.cpp
        src/app/population.h
        src/app/render.cpp
        src/app/render.h
        src/app/screen.cpp
        src/app/screen.h
        src/app/action.cpp
        src/app/action.h
        src/app/world.cpp
        src/app/world.h
        src/core/color.h
        src/core/config.cpp
        src/core/config.h
        src/core/geometry.cpp
        src/core/geometry.h
        src/core/id.h
        src/core/log.cpp
        src/core/log.h
        src/core/random.cpp
        src/core/random.h
        src/core/types.h
        src/platform/platform.cpp
        src/platform/platform.h
)

target_compile_options(
        lastditch
        PRIVATE
        -Wall
        -Wextra
        -Wno-missing-braces
)

add_custom_target(
        shaders
        COMMAND
        tools/compile_shaders.sh
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/compiled
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Regenerating SDL_GPU shader artifacts"
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
                ${CMAKE_SOURCE_DIR}/data
                $<TARGET_FILE_DIR:lastditch>/data
)
