#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float a_layer;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

out vec3 v_normal;
out vec2 v_uv;
out float v_layer;

void main()
{
     gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(a_position, 1.0);

     v_normal = a_normal;
     v_uv = a_uv;
     v_layer = a_layer;
}
