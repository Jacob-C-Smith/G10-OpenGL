// NOTE: This implementation was written 

#pragma once

#include <stdio.h>
#include <stdlib.h>


#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <G10/GXLinear.h>
#include <G10/arch/x86_64/GXAVXmath.h>

// This camera controller is based off the movement of a certain first person shooter game... Shhhhhh ;^)
struct GXCameraController_s
{
	// Camera object
	GXCamera_t *camera;

    float v_ang,
          h_ang,
          spdlim;

	// Displacement derivatives
    vec2        orientation;
	vec3		velocity,
				acceleration;
    
};

GXCameraController_t *camera_controller_from_camera ( GXInstance_t         *instance , GXCamera_t   *camera );
int                   update_controlee_camera       ( GXCameraController_t *camera_controller, float delta_time );
/*
int         update_camera_from_keyboard_input ( GXCamera_t *camera, const u8 *keyboardState, float delta_time )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(camera == (void*)0)
                goto noCamera;
            if (keyboardState == (void*)0)
                goto noKeyboardState;
        #endif  
    }

    // Uninitialized data
    vec2  orientation = { 0.f, 0.f };
    vec3 *velocity,
         *acceleration;

    // Initialized data

    // Turn keyboard input into orientation 
    {
        if (keyboardState[SDL_SCANCODE_W])
            if (keyboardState[SDL_SCANCODE_A])
                orientation = (vec2){ (float)-M_SQRT1_2, (float)M_SQRT1_2 }; // ↖ WA
            else if (keyboardState[SDL_SCANCODE_D])
                orientation = (vec2){ (float)M_SQRT1_2, (float)M_SQRT1_2 };  // ↗ WD
            else
                orientation = (vec2){ 0.f, 1.f };                            // ↑ W
        else if (keyboardState[SDL_SCANCODE_A])
            if (keyboardState[SDL_SCANCODE_S])
                orientation = (vec2){ (float)-M_SQRT1_2,(float)-M_SQRT1_2 }; // ↙ AS
            else
                orientation = (vec2){ -1.f, 0.f };                           // ← A
        else if (keyboardState[SDL_SCANCODE_D])
            if (keyboardState[SDL_SCANCODE_S])
                orientation = (vec2){ (float)M_SQRT1_2, (float)-M_SQRT1_2 }; // ↘ DS
            else
                orientation = (vec2){ 1.f, 0.f };                            // → D
        else if (keyboardState[SDL_SCANCODE_S])
            orientation = (vec2){ 0.f, -1.f };                               // ↓ S
        else
            return 0;
    }

    // Uncomment for orientation coords with newline
    // printf("( %.2f, %.2f )\n", orientation.x, orientation.y);
    
    // Uncomment for orientation coordinates without newline. 
    // printf("( %.2f, %.2f )\r", orientation.x, orientation.y);

    // Turn orientation into acceleration
    camera->acceleration.x = 1.f * orientation.x * camera->view.a + -orientation.y * 1.f * camera->view.c,
    camera->acceleration.y = 1.f * orientation.x * camera->view.e + -orientation.y * 1.f * camera->view.g;
    
    // Air resistance
    if (camera->velocity.x || camera->velocity.y)
        if (camera->velocity.x < 0.f)
            camera->acceleration.x += 0.001f;
        else
            camera->acceleration.x -= 0.001f;
        if (camera->velocity.y < 0.f)
            camera->acceleration.y += 0.001f;
        else
            camera->acceleration.y -= 0.001f;


    // Calculate v proj a                                                                                                                                                                                                                                                                                                                                      
    float n = AVXDot(&camera->acceleration, &camera->velocity);
    float d = length(camera->acceleration);
    d *= d;
    float c = length(camera->velocity);
    
    float cosTheta = n / (c * d);

    float lenvPa   = length(mul_vec3_f(camera->acceleration, n / d));
    float lenAdT   = length(mul_vec3_f(camera->acceleration, delta_time));
    float speedLim = 60000;

    if (lenvPa < speedLim - lenAdT)
    {
        camera->velocity = mul_vec3_f(camera->acceleration, delta_time);
    }
    else if (speedLim - lenAdT <= lenvPa < speedLim)
    {
    }

    camera->where.x += camera->velocity.x * delta_time,
    camera->where.y += camera->velocity.y * delta_time;

    vec3 tw;
    add_vec3(&tw, camera->target, camera->where);

    camera->view = look_at(camera->where, tw, camera->up);

    return 0;

    // Error handling
    {
        noCamera:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Parameter \"camera\" is null in call to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
        
        noKeyboardState:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Parameter \"keyboardState\" is null in call to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}*/