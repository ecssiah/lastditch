cmake_minimum_required(VERSION 3.20)

project(
        lastditch
        VERSION 0.1
        LANGUAGES C
)

# =========================================================
# Build approach
# =========================================================
#
# The game is written in ANSI C89 (ISO C90). The only language extension it
# uses is the single line `//` comment, so it is compiled as GNU C90
# (-std=gnu90).
#
# The game is built as a Unity build: CMake generates one source file that
# #includes every .c file listed below and compiles that single translation
# unit. LASTDITCH_UNITY_BUILD=OFF compiles the files one by one instead,
# which is useful to prove that every file includes what it needs.
#
# docs/build.md explains all of this in detail.

option(LASTDITCH_UNITY_BUILD "Compile each target as a single translation unit" ON)
option(LASTDITCH_BUILD_TESTS "Build the test program and register the checks with CTest" ON)

# =========================================================
# Third-party libraries
# =========================================================
#
# SDL3, SDL3_ttf and FreeType are vendored and built from source as static
# libraries. They are not C89 code; the language settings in this section
# apply to them only. The game's own targets override the standard below.

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(APPLE)
        enable_language(OBJC)
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

# =========================================================
# Python tooling
# =========================================================

if(WIN32)
        set(
                PROJECT_PYTHON_EXECUTABLE
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/.venv/Scripts/python.exe"
        )
else()
        set(
                PROJECT_PYTHON_EXECUTABLE
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/.venv/bin/python"
        )
endif()

if(NOT EXISTS "${PROJECT_PYTHON_EXECUTABLE}")
        message(FATAL_ERROR
                "The project Python environment does not exist.\n"
                "Run python3 tools/bootstrap.py (macOS/Linux) or "
                "py -3 tools/bootstrap.py (Windows) before configuring the project."
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

# =========================================================
# Generated reference tables
# =========================================================
#
# The block and nation reference tables are authored as YAML under data/ and
# turned into a C header (declarations) and a C source file (rows) each.

set(DATA_GENERATED_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(BLOCK_DATA_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/data/block_data.yml")
set(BLOCK_DATA_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/generate_block_data.py")
set(BLOCK_DATA_GENERATED_HEADER "${DATA_GENERATED_DIRECTORY}/block_data.generated.h")
set(BLOCK_DATA_GENERATED_SOURCE "${DATA_GENERATED_DIRECTORY}/block_data.generated.c")

set(ACTOR_DATA_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/data/actor_data.yml")
set(ACTOR_DATA_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/generate_actor_data.py")
set(ACTOR_DATA_GENERATED_HEADER "${DATA_GENERATED_DIRECTORY}/actor_data.generated.h")
set(ACTOR_DATA_GENERATED_SOURCE "${DATA_GENERATED_DIRECTORY}/actor_data.generated.c")

add_custom_command(
        OUTPUT
        "${BLOCK_DATA_GENERATED_HEADER}"
        "${BLOCK_DATA_GENERATED_SOURCE}"

        COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${DATA_GENERATED_DIRECTORY}"

        COMMAND
        "${Python3_EXECUTABLE}"
        "${BLOCK_DATA_GENERATOR}"
        "${BLOCK_DATA_CONFIG}"
        "${BLOCK_DATA_GENERATED_HEADER}"
        "${BLOCK_DATA_GENERATED_SOURCE}"

        DEPENDS
        "${BLOCK_DATA_GENERATOR}"
        "${BLOCK_DATA_CONFIG}"

        VERBATIM
)

add_custom_command(
        OUTPUT
        "${ACTOR_DATA_GENERATED_HEADER}"
        "${ACTOR_DATA_GENERATED_SOURCE}"

        COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${DATA_GENERATED_DIRECTORY}"

        COMMAND
        "${Python3_EXECUTABLE}"
        "${ACTOR_DATA_GENERATOR}"
        "${ACTOR_DATA_CONFIG}"
        "${ACTOR_DATA_GENERATED_HEADER}"
        "${ACTOR_DATA_GENERATED_SOURCE}"

        DEPENDS
        "${ACTOR_DATA_GENERATOR}"
        "${ACTOR_DATA_CONFIG}"

        VERBATIM
)

add_custom_target(
        generate_block_types
        DEPENDS
        "${BLOCK_DATA_GENERATED_HEADER}"
        "${BLOCK_DATA_GENERATED_SOURCE}"
)

add_custom_target(
        generate_actor_types
        DEPENDS
        "${ACTOR_DATA_GENERATED_HEADER}"
        "${ACTOR_DATA_GENERATED_SOURCE}"
)

# =========================================================
# Sources
# =========================================================
#
# The order of the .c files is the order in which the Unity build includes
# them. Nothing depends on it, because every file includes the headers it
# needs, but it is kept in dependency order for readability.

set(
        LASTDITCH_GENERATED_HEADERS
        "${ACTOR_DATA_GENERATED_HEADER}"
        "${BLOCK_DATA_GENERATED_HEADER}"
)

set(
        LASTDITCH_GENERATED_SOURCES
        "${ACTOR_DATA_GENERATED_SOURCE}"
        "${BLOCK_DATA_GENERATED_SOURCE}"
)

# Everything except the renderer and the program entry point: the part of
# the game that runs without a window, shared with the test program.
set(
        LASTDITCH_SIMULATION_SOURCES
        src/core/table.c
        src/core/log.c
        src/core/random.c
        src/core/color.c
        src/core/geometry/scalar.c
        src/core/geometry/vector.c
        src/core/geometry/quaternion.c
        src/core/geometry/orientation.c
        src/core/geometry/matrix.c
        src/core/geometry/bounds.c
        src/core/geometry/interpolation.c
        src/app/data/direction.c
        src/app/data/cell.c
        src/app/data/area.c
        src/app/data/blueprint.c
        src/app/data/nation.c
        src/app/data/actor.c
        src/app/world.c
        src/app/population.c
        src/app/physics.c
        src/app/navigation.c
        src/app/action.c
        src/platform/platform.c
        src/app/control.c
        src/app/screen.c
        src/app/debug.c
        src/app/mesh.c
)

set(
        LASTDITCH_APPLICATION_SOURCES
        src/app/render.c
        src/app/app.c
        src/lastditch.c
)

set(
        LASTDITCH_HEADERS
        src/core/types.h
        src/core/table.h
        src/core/log.h
        src/core/random.h
        src/core/color.h
        src/core/geometry.h
        src/core/geometry/scalar.h
        src/core/geometry/vector.h
        src/core/geometry/quaternion.h
        src/core/geometry/orientation.h
        src/core/geometry/matrix.h
        src/core/geometry/bounds.h
        src/core/geometry/interpolation.h
        src/app/data/constants.h
        src/app/data/direction.h
        src/app/data/cell.h
        src/app/data/area.h
        src/app/data/blueprint.h
        src/app/data/nation.h
        src/app/data/actor.h
        src/app/data/inputs.h
        src/app/data/work.h
        src/app/data/path.h
        src/app/data/render/vertex.h
        src/app/data/render/sector.h
        src/app/data/render/context.h
        src/app/world.h
        src/app/population.h
        src/app/physics.h
        src/app/navigation.h
        src/app/action.h
        src/app/control.h
        src/app/screen.h
        src/app/debug.h
        src/app/mesh.h
        src/app/render.h
        src/app/app.h
        src/platform/platform.h
)

# Applies the language dialect, the Unity build, the warnings, the include
# paths and the libraries shared by the game and its test program.
function(lastditch_configure_target target)
        set_target_properties(
                ${target}
                PROPERTIES
                C_STANDARD 90
                C_STANDARD_REQUIRED ON
                C_EXTENSIONS ON
                UNITY_BUILD ${LASTDITCH_UNITY_BUILD}
                UNITY_BUILD_BATCH_SIZE 0
        )

        add_dependencies(${target} generate_block_types generate_actor_types)

        target_link_libraries(
                ${target}
                PRIVATE
                SDL3_ttf::SDL3_ttf
                SDL3::SDL3
        )

        if(UNIX)
                target_link_libraries(${target} PRIVATE m)
        endif()

        target_include_directories(
                ${target}
                PRIVATE
                "${DATA_GENERATED_DIRECTORY}"
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

        # Third-party headers are not C89; treating them as system headers
        # keeps the warnings below focused on the game's own code.
        target_include_directories(
                ${target}
                SYSTEM PRIVATE
                $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:SDL3_ttf::SDL3_ttf,INTERFACE_INCLUDE_DIRECTORIES>
        )

        target_compile_options(
                ${target}
                PRIVATE
                -Wall
                -Wextra
                -Wno-missing-braces
                -Wdeclaration-after-statement
                -Wstrict-prototypes
                -Wold-style-definition
                -Wmissing-prototypes
                -Wvla
        )
endfunction()

# =========================================================
# Game
# =========================================================

add_executable(
        lastditch
        ${LASTDITCH_SIMULATION_SOURCES}
        ${LASTDITCH_APPLICATION_SOURCES}
        ${LASTDITCH_GENERATED_SOURCES}
        ${LASTDITCH_HEADERS}
        ${LASTDITCH_GENERATED_HEADERS}
)

lastditch_configure_target(lastditch)

# =========================================================
# Shaders
# =========================================================

set(SHADER_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders")
set(SHADER_OUTPUT_DIRECTORY "${SHADER_SOURCE_DIRECTORY}/compiled")
set(SHADER_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/compile_shaders.py")

file(
        GLOB SHADER_SOURCES
        CONFIGURE_DEPENDS
        "${SHADER_SOURCE_DIRECTORY}/*.vert.hlsl"
        "${SHADER_SOURCE_DIRECTORY}/*.frag.hlsl"
)

set(SHADER_OUTPUTS)

foreach(SHADER_SOURCE IN LISTS SHADER_SOURCES)
        get_filename_component(SHADER_FILENAME "${SHADER_SOURCE}" NAME)
        string(REGEX REPLACE "\\.hlsl$" "" SHADER_NAME "${SHADER_FILENAME}")

        set(SHADER_SPIRV "${SHADER_OUTPUT_DIRECTORY}/${SHADER_NAME}.spv")
        set(SHADER_MSL "${SHADER_OUTPUT_DIRECTORY}/${SHADER_NAME}.msl")
        set(SHADER_DXIL "${SHADER_OUTPUT_DIRECTORY}/${SHADER_NAME}.dxil")

        add_custom_command(
                OUTPUT
                "${SHADER_SPIRV}"
                "${SHADER_MSL}"

                BYPRODUCTS "${SHADER_DXIL}"

                COMMAND
                "${Python3_EXECUTABLE}"
                "${SHADER_COMPILER}"
                "${SHADER_SOURCE_DIRECTORY}"
                "${SHADER_OUTPUT_DIRECTORY}"
                "${SHADER_SOURCE}"

                DEPENDS
                "${SHADER_COMPILER}"
                "${SHADER_SOURCE}"

                COMMENT "Compiling SDL_GPU shader ${SHADER_FILENAME}"
                VERBATIM
        )

        list(APPEND SHADER_OUTPUTS "${SHADER_SPIRV}" "${SHADER_MSL}")
endforeach()

add_custom_target(shaders DEPENDS ${SHADER_OUTPUTS})

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

# =========================================================
# Tests and checks
# =========================================================
#
# `ctest` runs three things:
#   simulation     the test program, which exercises everything except the
#                  renderer and needs no window or GPU
#   documentation  the documentation standard (docs/documentation_standard.md)
#   c89            proof that the sources are ANSI C89 apart from // comments

if(LASTDITCH_BUILD_TESTS)
        enable_testing()

        set(
                LASTDITCH_TEST_SOURCES
                tests/test_main.c
                tests/test_core.c
                tests/test_data.c
                tests/test_world.c
                tests/test_simulation.c
                tests/test_presentation.c
        )

        add_executable(
                lastditch_tests
                ${LASTDITCH_SIMULATION_SOURCES}
                ${LASTDITCH_GENERATED_SOURCES}
                ${LASTDITCH_TEST_SOURCES}
                ${LASTDITCH_HEADERS}
                ${LASTDITCH_GENERATED_HEADERS}
                tests/test.h
        )

        lastditch_configure_target(lastditch_tests)

        target_include_directories(
                lastditch_tests
                PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )

        add_test(
                NAME simulation
                COMMAND lastditch_tests "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        )

        add_test(
                NAME documentation
                COMMAND
                "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/check_documentation.py"
                --root "${CMAKE_CURRENT_SOURCE_DIR}"
                "${CMAKE_CURRENT_SOURCE_DIR}/src"
                "${CMAKE_CURRENT_SOURCE_DIR}/tests"
                "${DATA_GENERATED_DIRECTORY}"
        )

        add_test(
                NAME c89
                COMMAND
                "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/check_c89.py"
                --compiler "${CMAKE_C_COMPILER}"
                --include "${CMAKE_CURRENT_SOURCE_DIR}/src"
                --include "${CMAKE_CURRENT_SOURCE_DIR}/tests"
                --include "${DATA_GENERATED_DIRECTORY}"
                --system-include "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3-3.4.14/include"
                --system-include "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3_ttf-3.2.2/include"
        )
endif()
