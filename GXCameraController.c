#include <G10/GXCameraController.h>

extern GXCameraController_t *active_camera_controller = 0;

GXCameraController_t* create_camera_controller()
{
    GXCameraController_t *ret = calloc(1, sizeof(GXCameraController_t));
    
    return ret;
    // TODO: Error handling
}

void camera_controller_forward      ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
    {

        // Key press
        if (state.inputs.key.depressed)
        {

        }

        // Key release
        else
        {

        }
    } 

}
void camera_controller_backward     ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
    {

        // Key press
        if (state.inputs.key.depressed)
        {

        }

        // Key release
        else
        {

        } 
    }
}
void camera_controller_strafe_left  ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
    {

        // Key press
        if (state.inputs.key.depressed)
        {

        }
        // Key release
        else
        {

        }
    } 

}
void camera_controller_strafe_right ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
    {

        // Key press
        if (state.inputs.key.depressed)
        {
            
        }

        // Key release
        else
        {

        }
    }

}

void camera_controller_up           ( callback_parameter_t state, GXInstance_t* instance ) 
{
    if (state.input_state == MOUSE)
        active_camera_controller->v_ang -= 0.05f * instance->deltaTime;

}
void camera_controller_down         ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        active_camera_controller->v_ang += 0.05f * instance->deltaTime;

}
void camera_controller_left         ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        active_camera_controller->h_ang -= 0.05f * instance->deltaTime;

}
void camera_controller_right        ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        active_camera_controller->h_ang += 0.05f * instance->deltaTime;

}


int camera_controller_from_camera ( GXInstance_t* instance, GXCamera_t *camera )
{
    // TODO: Argument check
    if (active_camera_controller)
        ;
        // TODO: FREE;

    active_camera_controller = create_camera_controller();
    active_camera_controller->camera = camera;
    // Displacement binds
    GXBind_t *forward      = find_bind(instance->input, "FORWARD"),
             *backward     = find_bind(instance->input, "BACKWARD"),
             *left         = find_bind(instance->input, "STRAFE LEFT"),
             *right        = find_bind(instance->input, "STRAFE RIGHT"),

    // Orientation binds
             *orient_up    = find_bind(instance->input, "UP"),
             *orient_down  = find_bind(instance->input, "DOWN"),
             *orient_left  = find_bind(instance->input, "LEFT"),
             *orient_right = find_bind(instance->input, "RIGHT");
    
    // Assign displacement callbacks
    register_bind_callback(forward     , &camera_controller_forward);
    register_bind_callback(backward    , &camera_controller_backward);
    register_bind_callback(left        , &camera_controller_strafe_left);
    register_bind_callback(right       , &camera_controller_strafe_right);

    // Assign orientation callbacks
    register_bind_callback(orient_up   , &camera_controller_up);
    register_bind_callback(orient_down , &camera_controller_down);
    register_bind_callback(orient_left , &camera_controller_left);
    register_bind_callback(orient_right, &camera_controller_right);

    return 0;
    
    // TODO: Error handling
}

int update_controlee_camera ( float delta_time ) 
{
    // Checks
    {
        if (active_camera_controller == 0)
            goto noActiveCameraController;
    }

    // Initialized data
    vec2                  l_orient   = active_camera_controller->orientation;
    vec3                  target = (vec3){ 0.f, 0.f, 0.f };
    GXCameraController_t *controller = active_camera_controller;
    GXCamera_t           *camera     = active_camera_controller->camera;

    float                 n            = 0,
                          d            = 0,
                          c            = 0,
                          cos_theta    = 0,
                          len_v_proj_a = 0,
                          len_a_proj_t = 0,
                          speed_lim    = 10;

    printf("< %f, %f >                           \r", active_camera_controller->h_ang, active_camera_controller->v_ang);

    if (controller->v_ang > (float)M_PI_2 - 0.0001f)
        controller->v_ang = (float)M_PI_2 - 0.0001f;
    if (controller->v_ang < (float)-M_PI_2 + 0.0001f)
        controller->v_ang = (float)-M_PI_2 + 0.0001f;

    if (controller->h_ang > (float)M_PI - 0.0001f)
        controller->h_ang = (float)-M_PI + 0.0001f;
    if (controller->h_ang < (float)-M_PI + 0.0001f)
        controller->h_ang = (float)M_PI - 0.0001f;


    // Compute new target from vertical and horizontal angles
    camera->target.x = sinf(controller->h_ang) * cosf(controller->v_ang);
    camera->target.y = cosf(controller->h_ang) * cosf(controller->v_ang);
    camera->target.z = sinf(-controller->v_ang);

    // Define and populate target where vector
    vec3 tw;
    add_vec3(&tw, camera->target, camera->where);

    // look at
    camera->view = look_at(camera->where, tw, camera->up);

    return 0;

    // Error handling
    {
        noActiveCameraController:
        #ifndef NDEBUG
            g_print_warning("[G10] [Camera controller] No active camera, set a camera with \"camera_controller_from_camera()\"\n");        
        #endif
        return 0;
    }
}
