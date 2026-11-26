# SDL_GPU compiles Metal Shading Language at runtime.
SPIRV_CROSS ?= spirv-cross
SHADER_OUTPUTS = $(SPIRV_SHADERS:.spv=.msl)

# The HLSL binding numbers match SDL's Metal resource slots.
$(SHADER_OUTPUT_DIR)/%.msl: $(SHADER_OUTPUT_DIR)/%.spv $(BUILD_RULES)
	$(SPIRV_CROSS) "$<" --msl --msl-decoration-binding --output "$@"
