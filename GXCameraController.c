#include <G10/GXCameraController.h>

float x_orient,
      y_orient,
      h_ang,
      v_ang;

GXCameraController_t* create_camera_controller       ( void )
{
    GXCameraController_t *ret = calloc(1, sizeof(GXCameraController_t));
    
    // Check allocaated memory
    {
        #ifndef NDEBUG
            if(ret==(void *)0)
                goto noMem;
        #endif
    }

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera controller] Unable to allocate memory for camera controller\n");
        #endif
        return 0;
    }
}

void                  camera_controller_forward      ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
        y_orient = (state.inputs.key.depressed) ? 1 : 0;


}
void                  camera_controller_backward     ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
        y_orient = (state.inputs.key.depressed) ? -1 : 0;
}
void                  camera_controller_strafe_left  ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
        x_orient = (state.inputs.key.depressed) ? -1 : 0;
}
void                  camera_controller_strafe_right ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == KEYBOARD)
        x_orient = (state.inputs.key.depressed) ? 1 : 0;
}

void                  camera_controller_up           ( callback_parameter_t state, GXInstance_t* instance ) 
{
    if (state.input_state == MOUSE)
        v_ang -= 0.0024f;

}
void                  camera_controller_down         ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        v_ang += 0.0024f;

}
void                  camera_controller_left         ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        h_ang -= 0.0024f;

}
void                  camera_controller_right        ( callback_parameter_t state, GXInstance_t* instance )
{
    if (state.input_state == MOUSE)
        h_ang += 0.0024f;

}

GXCameraController_t *camera_controller_from_camera  ( GXInstance_t* instance, GXCamera_t *camera )
{
    // Argument check
    {
        #ifndef NDEBUG
            if ( instance == (void *)0 )
                goto no_instance;
            if (camera == (void *)0 )
                goto no_camera;
        #endif
    }

    GXCameraController_t *ret = create_camera_controller();;
    u8 errors = 0;
    ret->camera = camera;

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
    
    // Error checking
    {
        // This error checking is a little bit unorthadox, so I figure its worth explaining whats going on.
        // 8 binds are required to set up a camera controller, and there are eight bits in a byte. If one of
        // the binds is missing, a bit is flipped. If there are any errors, the errors byte will be nonzero.

        // The error byte is checked for nonzero value at the end. If there is a nonzero value, the program 
        // branches to error handling. Else, continue

        #ifndef NDEBUG

        if (forward      == (void*)0)
            errors |= 0x1;
        if (backward     == (void*)0)
            errors |= 0x2;
        if (left         == (void*)0)
            errors |= 0x4;
        if (right        == (void*)0)
            errors |= 0x8;
        if (orient_up    == (void*)0)
            errors |= 0x10;
        if (orient_down  == (void*)0)
            errors |= 0x20;
        if (orient_left  == (void*)0)
            errors |= 0x40;
        if (orient_right == (void*)0)
            errors |= 0x80;


        if (errors)
            goto missing_binds;

        #endif
    }

    ret->spdlim = 0.25;

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

    ret->camera = camera;

    return ret;
    
    // Error handling
    {

        // Argument error
        {
            no_instance:
            #ifndef NDEBUG
                g_print_error("[G10] [Camera Controller] Null pointer provided for \"instance\" in call to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
            no_camera:
            #ifndef NDEBUG
                g_print_error("[G10] [Camera Controller] Null pointer provided for \"camera\" in call to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

        // Missing bind for camera controller
        {
            missing_binds:
            if (errors & 0x1)
                g_print_error("[G10] [Camera controller] Missing \"FORWARD\" input bind to set up camera controller\n");
            if ( errors & 0x2  )
                g_print_error("[G10] [Camera controller] Missing \"BACKWARD\" input bind to set up camera controller\n");
            if ( errors & 0x4  )
                g_print_error("[G10] [Camera controller] Missing \"STRAFE LEFT\" input bind to set up camera controller\n");
            if ( errors & 0x8  )
                g_print_error("[G10] [Camera controller] Missing \"STRAFE RIGHT\" input bind to set up camera controller\n");
            if ( errors & 0x10 )
                g_print_error("[G10] [Camera controller] Missing \"UP\" input bind to set up camera controller\n");
            if ( errors & 0x20 )
                g_print_error("[G10] [Camera controller] Missing \"DOWN\" input bind to set up camera controller\n");
            if ( errors & 0x40 )
                g_print_error("[G10] [Camera controller] Missing \"LEFT\" input bind to set up camera controller\n");
            if ( errors & 0x80 )
                g_print_error("[G10] [Camera controller] Missing \"RIGHT\" input bind to set up camera controller\n");
            
            // TODO: Free camera controller

            return 0;

        }

    }
}

int                   update_controlee_camera        ( GXCameraController_t *camera_controller, float delta_time ) 
{
    // Checks
    {
        if (camera_controller == 0)
            goto noActiveCameraController;
    }

    // Initialized data
    vec2                  l_orient;
    vec3                  target = (vec3){ 0.f, 0.f, 0.f };
    GXCamera_t           *camera     = camera_controller->camera;

    float                 n            = 0,
                          d            = 0,
                          c            = 0,
                          cos_theta    = 0,
                          len_v_proj_a = 0,
                          len_a_proj_t = 0,
                          speed_lim    = 10;

    l_orient = camera_controller->orientation;

    // Clamp vertical angle 
    if (v_ang > (float)M_PI_2 - 0.00025f)
        v_ang = (float)M_PI_2 - 0.00025f;
    if (v_ang < (float)-M_PI_2 + 0.00025f)
        v_ang = (float)-M_PI_2 + 0.00025f;

    // Clamp horizontal angle
    if (h_ang > (float)M_PI - 0.00025f)
        h_ang = (float)-M_PI + 0.00025f;
    if (h_ang < (float)-M_PI + 0.00025f)
        h_ang = (float)M_PI - 0.00025f;

    camera_controller->orientation = (vec2){ x_orient, y_orient };
    camera_controller->v_ang = v_ang;
    camera_controller->h_ang = h_ang;

    // Compute new target from vertical and horizontal angles
    camera->target.x = sinf(camera_controller->h_ang) * cosf(camera_controller->v_ang);
    camera->target.y = cosf(camera_controller->h_ang) * cosf(camera_controller->v_ang);
    camera->target.z = sinf(-camera_controller->v_ang);

    float sl = camera_controller->spdlim;

    // Turn orientation into movement
    camera->location.x += sl * (float)l_orient.x * camera->view.a + -(float)l_orient.y * sl * camera->view.c,
    camera->location.y += sl * (float)l_orient.x * camera->view.e + -(float)l_orient.y * sl * camera->view.g;
    camera->location.z += sl * (float)l_orient.x * camera->view.i + -(float)l_orient.y * sl * camera->view.k;

    // Define and populate target where vector
    vec3 tw;
    add_vec3(&tw, camera->target, camera->location);

    // look at
    camera->view = look_at(camera->location, tw, camera->up);

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