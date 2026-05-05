#version 410 core

layout (location = 0) in uint a_vertex;
layout (location = 1) in uint a_face;

uniform vec3 u_normal_table[6];
uniform vec3 u_uv_projection_table[6 * 2];

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

flat out vec3 v_normal;
     out vec2 v_uv;
flat out uint v_block_type;

void main()
{
	vec3 vertex_coordinate;
	vertex_coordinate.x = float((a_vertex >>  0u) &  63u);
	vertex_coordinate.y = float((a_vertex >>  6u) &  63u);
	vertex_coordinate.z = float((a_vertex >> 12u) & 255u);

	uint block_type = ((a_face >>  0u) & 255u);
	uint direction =  ((a_face >>  8u) &   7u);

	vec2 uv;
	uv.s = dot(vertex_coordinate, u_uv_projection_table[2 * direction + 0]);
	uv.t = dot(vertex_coordinate, u_uv_projection_table[2 * direction + 1]);

	vec3 normal = u_normal_table[direction];

	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(vertex_coordinate, 1.0);

	v_normal = normal;
	v_uv = uv;
	v_block_type = block_type - 1;
}
