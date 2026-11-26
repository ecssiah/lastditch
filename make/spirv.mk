GLSLANG ?= glslangValidator
GLSLANG_FLAGS := -D -V --target-env vulkan1.0 -e main
SPIRV_SHADERS := $(patsubst $(SHADER_SOURCE_DIR)/%.hlsl,$(SHADER_OUTPUT_DIR)/%.spv,$(VERTEX_SHADERS) $(FRAGMENT_SHADERS))

$(SHADER_OUTPUT_DIR)/%.vert.spv: $(SHADER_SOURCE_DIR)/%.vert.hlsl $(BUILD_RULES)
	@mkdir -p "$(@D)"
	$(GLSLANG) $(GLSLANG_FLAGS) -S vert -o "$@" "$<"

$(SHADER_OUTPUT_DIR)/%.frag.spv: $(SHADER_SOURCE_DIR)/%.frag.hlsl $(BUILD_RULES)
	@mkdir -p "$(@D)"
	$(GLSLANG) $(GLSLANG_FLAGS) -S frag -o "$@" "$<"
