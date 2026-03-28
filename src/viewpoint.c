#include "viewpoint.h"

#include "jsk.h"

#include "ld_data.h"
#include "input.h"

void viewpoint_get_forward(Sim *sim, vec3 out_forward)
{
    Viewpoint *viewpoint = &sim->viewpoint;
    
    const f32 rotation_x = glm_rad(viewpoint->rotation[0]);
    const f32 rotation_z = glm_rad(viewpoint->rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void viewpoint_get_right(Sim *sim, vec3 out_right)
{
    Viewpoint *viewpoint = &sim->viewpoint;
    
    vec3 forward;
    viewpoint_get_forward(sim, forward);

    glm_vec3_cross(GLM_ZUP, forward, out_right);

    glm_vec3_normalize(out_right);
}

void viewpoint_get_up(Sim *sim, vec3 out_up)
{
    Viewpoint *viewpoint = &sim->viewpoint;
    
    vec3 forward;
    viewpoint_get_forward(sim, forward);

    vec3 right;
    viewpoint_get_right(sim, right);

    glm_vec3_cross(forward, right, out_up);
}

void viewpoint_get_projection_matrix(Sim *sim, mat4 out_projection_matrix)
{
    Viewpoint *viewpoint = &sim->viewpoint;
    
    const f32 aspect_ratio = (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT;
    
    glm_perspective(
        glm_rad(60.0f),
        aspect_ratio,
        0.1f,
        1000.0f,
        out_projection_matrix
    );
}

void viewpoint_get_view_matrix(Sim *sim, mat4 out_view_matrix)
{
    Viewpoint *viewpoint = &sim->viewpoint;
    
    vec3 forward;
    viewpoint_get_forward(sim, forward);
    
    vec3 center;
    glm_vec3_add(viewpoint->world_position, forward, center);
    
    glm_lookat(viewpoint->world_position, center, GLM_ZUP, out_view_matrix);
}

void viewpoint_init(Sim *sim)
{
    Viewpoint *viewpoint = &sim->viewpoint;

    viewpoint->world_position[0] = WORLD_CENTER;
    viewpoint->world_position[1] = WORLD_CENTER;
    viewpoint->world_position[2] = TOWER_ROOF_HEIGHT + 5;

    viewpoint->rotation[0] = 0.0f;
    viewpoint->rotation[1] = 0.0f;
    viewpoint->rotation[2] = 90.0f;

    viewpoint->speed = 16.0f;
    viewpoint->sensitivity = 0.1f;

    viewpoint_get_projection_matrix(sim, viewpoint->projection_matrix);
}

void viewpoint_update(Sim *sim, Shell *shell)
{
    Viewpoint *viewpoint = &sim->viewpoint;

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
    
    vec3 viewpoint_forward;
    vec3 viewpoint_right;

    viewpoint_get_right(sim, viewpoint_right);
    glm_vec3_scale(viewpoint_right, input_value[0], velocity_right);

    viewpoint_get_forward(sim, viewpoint_forward);
    glm_vec3_scale(viewpoint_forward, input_value[1], velocity_forward);
    
    glm_vec3_scale(GLM_ZUP, input_value[2], velocity_up);
    
    vec3 velocity = { 0, 0, 0 };

    glm_vec3_add(velocity, velocity_right, velocity);
    glm_vec3_add(velocity, velocity_forward, velocity);
    glm_vec3_add(velocity, velocity_up, velocity);
    
    glm_vec3_scale(velocity, viewpoint->speed * shell->delta_time, velocity);

    glm_vec3_add(viewpoint->world_position, velocity, viewpoint->world_position);
    
    viewpoint->rotation[0] -= viewpoint->sensitivity * input->mouse_delta_y;
    viewpoint->rotation[2] -= viewpoint->sensitivity * input->mouse_delta_x;
    
    if (viewpoint->rotation[0] > PITCH_LIMIT)
    {
        viewpoint->rotation[0] = PITCH_LIMIT;
    }
    
    if (viewpoint->rotation[0] < -PITCH_LIMIT)
    {
        viewpoint->rotation[0] = -PITCH_LIMIT;
    }
    
    viewpoint_get_view_matrix(sim, viewpoint->view_matrix);
}
