#version 410 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;

uniform mat4 u_projection_matrix;

out vec2 v_uv;

void main()
{
    gl_Position = u_projection_matrix * vec4(a_position, 0.0, 1.0);
    
    v_uv = a_uv;
}