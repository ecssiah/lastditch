cmake_minimum_required(VERSION 3.20)

project(
        lastditch
        VERSION 0.1
        LANGUAGES C CXX
)

include(CTest)

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

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_OPENGL OFF CACHE BOOL "" FORCE)
set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)

add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3-3.4.14
        EXCLUDE_FROM_ALL
)

add_executable(lastditch src/lastditch.cpp)

target_link_libraries(
        lastditch
        PRIVATE
        SDL3::SDL3
)

target_include_directories(
        lastditch
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(
        lastditch
        PRIVATE
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
        src/app/work.cpp
        src/app/work.h
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
                ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.sh
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/compiled
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Regenerating SDL_GPU shader artifacts"
)

if (BUILD_TESTING)
        add_executable(
                projection_test
                tests/projection_test.cpp
                src/core/geometry.cpp
        )
        target_include_directories(projection_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        add_test(NAME projection_depth_range COMMAND projection_test)
endif ()

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
