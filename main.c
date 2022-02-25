// Standard library
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// SDL2
#include <SDL2/SDL.h>

// OpenGL
#include <glad/glad.h>

// G10
#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <G10/GXScene.h>
#include <G10/GXNetworking.h>
#include <G10/GXInput.h>
#include <G10/GXCameraController.h>


// G10 arch
#ifdef _M_X64
// x86_64
#include <G10/arch/x86_64/GXAVXmath.h>
#include <G10/arch/x86_64/GXAVXlinear.h>
#elif _M_ARM64
// ARM specific includes
#endif

// For some reason, SDL defines main. I don't know why, but it needs to be undef'd.
#undef main

int main ( int argc, const char *argv[] )
{

    // Initialized G10 data
    GXInstance_t         *instance          = 0;
    GXScene_t            *scene             = 0;
    GXCameraController_t *camera_controller = 0;
    char                 *initial_instance  = 0;

    // Parse command line arguments
    {
        // Iterate through arguments
        for (size_t i = 1; i < argc; i++)
        {
            // Help 
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                // Initialize SDL
                if (SDL_Init(SDL_INIT_EVERYTHING))
                {
                    printf("Failed to initialize SDL\n");
                    return 0;
                }
                 
                SDL_OpenURL("README.md");

                return 0;
            }

            // Load
            if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--load") == 0)
            {
                initial_instance = argv[++i];
                continue;
            }

        }
    }

    // Initialize G10
    {
        #ifndef NDEBUG
            instance = g_init(initial_instance ? initial_instance : "G10/Debug instance.json");
        #else 
            instance = g_init(initial_instance ? initial_instance : "G10/Release instance.json");
        #endif
    }

    // Set up some binds
    {
        // Find the quit bind and the mouse locking bind
        GXBind_t *quit       = find_bind(instance->input, "QUIT"),
                 *lock_mouse = find_bind(instance->input, "TOGGLE LOCK MOUSE"),
                 *fullscreen = find_bind(instance->input, "TOGGLE FULLSCREEN");

        scene = instance->scenes;

        // If quit is fired, exit the game loop
        register_bind_callback(quit, &g_exit_game_loop);

        // Toggle mouse locking
        register_bind_callback(lock_mouse, &g_toggle_mouse_lock);

        // Toggle fullscreen
        register_bind_callback(fullscreen, &g_toggle_full_screen);

        // Set up the camera controller from the primary camera.
        // NOTE: Requires the following binds: FORWARD, BACKWARD, STRAFE LEFT, STRAFE RIGHT, UP, DOWN, LEFT, and RIGHT. 
        camera_controller = camera_controller_from_camera(instance, scene->cameras);
        
    }

    instance->running = true;

    g_window_resize(instance);

    // Main game loop
    while (instance->running)
    {
        // Compute delta time
        g_delta(instance);

        // Debug only FPS readout
        {
            #ifndef NDEBUG
                printf("FPS: %.1f\r", (float)instance->delta_time * (float)1000.f); // Uses CR instead of CR LF so as to provide a quasi realtime FPS readout
            #endif
        }

        // Process input
        process_input(instance);

        // Clear the screen
        g_clear();

        // Compute physics and draw the scene
        if (instance->scenes)
        {
            update_controlee_camera(camera_controller, instance->delta_time);
            compute_physics(scene, instance->delta_time);

            draw_scene(instance->scenes);
        }

        // Swap the window 
        g_swap(instance);
    }

    // G10 Unloading
    {
        g_exit(instance);
    }

     return 0;
}
