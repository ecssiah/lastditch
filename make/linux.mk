# SDL_GPU's Vulkan backend consumes SPIR-V on Linux.
GLSLANG ?= glslangValidator
PLATFORM_LDLIBS := -lm

SHADER_OUTPUTS = $(patsubst $(SHADER_SOURCE_DIR)/%.hlsl,$(SHADER_OUTPUT_DIR)/%.spv,$(VERTEX_SHADERS) $(FRAGMENT_SHADERS))
GLSLANG_FLAGS := -D -V --target-env vulkan1.0 -e main

$(SHADER_OUTPUT_DIR)/%.vert.spv: $(SHADER_SOURCE_DIR)/%.vert.hlsl $(BUILD_RULES)
	@mkdir -p "$(@D)"
	$(GLSLANG) $(GLSLANG_FLAGS) -S vert -o "$@" "$<"

$(SHADER_OUTPUT_DIR)/%.frag.spv: $(SHADER_SOURCE_DIR)/%.frag.hlsl $(BUILD_RULES)
	@mkdir -p "$(@D)"
	$(GLSLANG) $(GLSLANG_FLAGS) -S frag -o "$@" "$<"
