# Last Ditch development build. GNU Make 4.3 or newer is required.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

BUILD_DIR ?= build
PKG_CONFIG ?= pkg-config
PLATFORM ?= $(shell uname -s)

SDL_PACKAGES := sdl3-ttf sdl3

# SDL 3.2 uses SDL3_image to load PNG textures.
ifeq ($(shell $(PKG_CONFIG) --exists 'sdl3 < 3.4.0' 2>/dev/null && echo yes),yes)
SDL_PACKAGES := sdl3-image $(SDL_PACKAGES)
endif

SDL_CPPFLAGS := $(shell $(PKG_CONFIG) --cflags $(SDL_PACKAGES) 2>/dev/null)
SDL_LDLIBS := $(shell $(PKG_CONFIG) --libs $(SDL_PACKAGES) 2>/dev/null)

WARNINGS := -Wall \
	-Wextra \
	-Wno-missing-braces \
	-Wdeclaration-after-statement \
	-Wstrict-prototypes \
	-Wold-style-definition \
	-Wmissing-prototypes \
	-Wvla

CFLAGS ?= -g

SIMULATION_SOURCES := src/core/table.c \
	src/core/log.c \
	src/core/random.c \
	src/core/color.c \
	src/core/geometry/scalar.c \
	src/core/geometry/vector.c \
	src/core/geometry/quaternion.c \
	src/core/geometry/orientation.c \
	src/core/geometry/matrix.c \
	src/core/geometry/bounds.c \
	src/core/geometry/interpolation.c \
	src/app/data/actor_reference.c \
	src/app/data/block_reference.c \
	src/app/data/direction.c \
	src/app/data/cell.c \
	src/app/data/area.c \
	src/app/data/blueprint.c \
	src/app/data/nation.c \
	src/app/data/actor.c \
	src/app/world.c \
	src/app/population.c \
	src/app/physics.c \
	src/app/navigation.c \
	src/app/action.c \
	src/platform/platform.c \
	src/app/control.c \
	src/app/screen.c \
	src/app/debug.c \
	src/app/mesh.c

GAME_SOURCES := src/app/render.c \
	src/app/app.c \
	src/lastditch.c

TEST_SOURCES := tests/test_main.c \
	tests/test_core.c \
	tests/test_data.c \
	tests/test_world.c \
	tests/test_simulation.c \
	tests/test_presentation.c

COMMON_OBJECTS := $(addprefix $(BUILD_DIR)/obj/,$(SIMULATION_SOURCES:.c=.o))
GAME_OBJECTS := $(addprefix $(BUILD_DIR)/obj/,$(GAME_SOURCES:.c=.o)) $(COMMON_OBJECTS)
TEST_OBJECTS := $(addprefix $(BUILD_DIR)/obj/,$(TEST_SOURCES:.c=.o)) $(COMMON_OBJECTS)

GAME := $(BUILD_DIR)/lastditch$(EXE_SUFFIX)
TEST := $(BUILD_DIR)/lastditch_tests$(EXE_SUFFIX)

SHADER_SOURCE_DIR := assets/shaders
SHADER_OUTPUT_DIR := $(BUILD_DIR)/$(SHADER_SOURCE_DIR)/compiled

VERTEX_SHADERS := $(wildcard $(SHADER_SOURCE_DIR)/*.vert.hlsl)
FRAGMENT_SHADERS := $(wildcard $(SHADER_SOURCE_DIR)/*.frag.hlsl)

RUNTIME_FILES := $(shell find assets data -path assets/shaders/compiled -prune -o -type f -print)
STAGED_FILES := $(addprefix $(BUILD_DIR)/,$(RUNTIME_FILES))
BUILD_RULES := Makefile make/spirv.mk

ifeq ($(PLATFORM),Linux)
BUILD_RULES += make/linux.mk
include make/linux.mk
else ifeq ($(PLATFORM),Darwin)
BUILD_RULES += make/macos.mk
include make/macos.mk
else
$(error Make build supports Linux and macOS; detected $(PLATFORM))
endif
include make/spirv.mk

.PHONY: all build run shaders check check-sdl
all: $(GAME)

build: all

run: all
	$(GAME)

shaders: $(SHADER_OUTPUTS)

check: $(TEST)
	$(TEST) "$(CURDIR)/assets"

check-sdl:
	@$(PKG_CONFIG) --print-errors --exists $(SDL_PACKAGES) || { \
	    echo "SDL development packages are required. See build.md." >&2; exit 1; }

$(BUILD_DIR)/obj/%.o: %.c $(BUILD_RULES) | check-sdl
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(SDL_CPPFLAGS) -Isrc -Itests -Idata $(CFLAGS) -std=gnu90 $(WARNINGS) -MMD -MP -c "$<" -o "$@"

$(GAME): $(GAME_OBJECTS) | $(STAGED_FILES) shaders
	$(CC) $(LDFLAGS) -o "$@" $^ $(SDL_LDLIBS) $(PLATFORM_LDLIBS) $(LDLIBS)

$(TEST): $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) -o "$@" $^ $(SDL_LDLIBS) $(PLATFORM_LDLIBS) $(LDLIBS)

$(STAGED_FILES): $(BUILD_DIR)/%: %
	@mkdir -p "$(@D)"
	cp "$<" "$@"

-include $(sort $(GAME_OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d))
