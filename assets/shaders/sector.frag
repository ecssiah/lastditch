#version 410 core

flat in vec3 v_normal;
     in vec2 v_uv;
flat in uint v_block_type;

uniform sampler2DArray u_texture_sampler;

out vec4 FragColor;

void main()
{
	vec4 color = texture(u_texture_sampler, vec3(v_uv.s, v_uv.t, float(v_block_type)));		

	FragColor = color;
}