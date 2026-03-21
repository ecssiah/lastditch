#version 410 core

in vec3 v_normal;
in vec2 v_uv;
in float v_layer;

out vec4 FragColor;

uniform sampler2DArray u_texture_sampler;

void main()
{
    vec3 texture_color = texture(u_texture_sampler, vec3(v_uv, v_layer)).rgb;

    FragColor = vec4(texture_color, 1.0);
}
