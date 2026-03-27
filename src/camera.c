#include "camera.h"

#include "jsk.h"

#include "ld_data.h"
#include "input.h"

void camera_get_forward(Sim *sim, vec3 out_forward)
{
    Camera *camera = &sim->camera;
    
    const f32 rotation_x = glm_rad(camera->rotation[0]);
    const f32 rotation_z = glm_rad(camera->rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void camera_get_right(Sim *sim, vec3 out_right)
{
    Camera *camera = &sim->camera;
    
    vec3 forward;
    camera_get_forward(sim, forward);

    glm_vec3_cross(GLM_ZUP, forward, out_right);

    glm_vec3_normalize(out_right);
}

void camera_get_up(Sim *sim, vec3 out_up)
{
    Camera *camera = &sim->camera;
    
    vec3 forward;
    camera_get_forward(sim, forward);

    vec3 right;
    camera_get_right(sim, right);

    glm_vec3_cross(forward, right, out_up);
}

void camera_get_projection_matrix(Sim *sim, mat4 out_projection_matrix)
{
    Camera *camera = &sim->camera;
    
    const f32 aspect_ratio = (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT;
    
    glm_perspective(
	glm_rad(60.0f),
	aspect_ratio,
	0.1f,
	1000.0f,
	out_projection_matrix
    );
}

void camera_get_view_matrix(Sim *sim, mat4 out_view_matrix)
{
    Camera *camera = &sim->camera;
    
    vec3 forward;
    camera_get_forward(sim, forward);
    
    vec3 center;
    glm_vec3_add(camera->world_position, forward, center);
    
    glm_lookat(camera->world_position, center, GLM_ZUP, out_view_matrix);
}

void camera_init(Sim *sim)
{
    Camera *camera = &sim->camera;
    
    camera->world_position[0] = (f32)WORLD_SIZE_IN_CELLS / 2.0f;
    camera->world_position[1] = (f32)WORLD_SIZE_IN_CELLS / 2.0f;
    camera->world_position[2] = (f32)TOWER_ROOF_HEIGHT + 5.0f;

    camera->rotation[0] = 0.0f;
    camera->rotation[1] = 0.0f;
    camera->rotation[2] = 90.0f;

    camera->speed = 16.0f;
    camera->sensitivity = 0.1f;

    camera_get_projection_matrix(sim, camera->projection_matrix);
}

void camera_update(Sim *sim, Shell *shell)
{
    Camera *camera = &sim->camera;

    Input *input = &shell->input;
  
    vec3 input_value = { 0, 0, 0 };

    if (input_key_is_down(shell, GLFW_KEY_A))
    {
	input_value[0] += 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_D))
    {
	input_value[0] -= 1.0f;
    }

    if (input_key_is_down(shell, GLFW_KEY_W))
    {
	input_value[1] += 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_S))
    {
	input_value[1] -= 1.0f;
    }
    
    glm_vec3_normalize(input_value);

    if (input_key_is_down(shell, GLFW_KEY_Q))
    {
	input_value[2] -= 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_E))
    {
	input_value[2] += 1.0f;
    }

    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;
    
    vec3 camera_forward;
    vec3 camera_right;

    camera_get_right(sim, camera_right);
    glm_vec3_scale(camera_right, input_value[0], velocity_right);

    camera_get_forward(sim, camera_forward);
    glm_vec3_scale(camera_forward, input_value[1], velocity_forward);
    
    glm_vec3_scale(GLM_ZUP, input_value[2], velocity_up);
    
    vec3 velocity = { 0, 0, 0 };

    glm_vec3_add(velocity, velocity_right, velocity);
    glm_vec3_add(velocity, velocity_forward, velocity);
    glm_vec3_add(velocity, velocity_up, velocity);
    
    glm_vec3_scale(velocity, camera->speed * shell->delta_time, velocity);

    glm_vec3_add(camera->world_position, velocity, camera->world_position);
    
    camera->rotation[0] -= camera->sensitivity * input->mouse_delta_y;
    camera->rotation[2] -= camera->sensitivity * input->mouse_delta_x;

    const f32 pitch_limit = 89.9f;
    
    if (camera->rotation[0] > pitch_limit)
    {
	camera->rotation[0] = pitch_limit;
    }
    
    if (camera->rotation[0] < -pitch_limit)
    {
	camera->rotation[0] = -pitch_limit;
    }
    
    camera_get_view_matrix(sim, camera->view_matrix);
}
