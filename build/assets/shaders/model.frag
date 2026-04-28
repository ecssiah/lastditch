#version 410 core

in vec3 v_normal;
in vec2 v_uv;

uniform sampler2DArray u_texture_sampler;

uniform int u_texture_layer;

out vec4 FragColor;

void main()
{
    vec4 color = texture(u_texture_sampler, vec3(v_uv, float(u_texture_layer)));		

    FragColor = color;
}