#version 410 core

in vec2 v_uv;

uniform sampler2D u_font_texture_sampler;

out vec4 FragColor;

void main()
{
    vec4 texture_color = texture(u_font_texture_sampler, v_uv);
    
    FragColor = texture_color;
}