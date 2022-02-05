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

// For some reason, SDL defines main. I don't know why, but we have to undef it.
#undef main

int main ( int argc, const char *argv[] )
{

    // Initialized G10 data
    GXInstance_t *instance     = 0;
    GXScene_t    *scene        = 0;
    char         *initialScene = 0,
                 *initialIP    = 0;

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
                initialScene = argv[++i];
                continue;
            }

            // Connect
            if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i],"--connect") == 0)
            {
                initialIP = argv[++i];
                continue;
            }
        }
    }

    // Initialize G10
    {
        #ifndef NDEBUG
            instance = g_init("G10/Debug instance.json");
        #else 
            instance = g_init("G10/Release inscance.json");
        #endif
    }

    // Create a splash screen
    {
        #ifdef NDEBUG
            create_splashscreen("G10/splash/splash front.png", "G10/splash/splash back.png");
        #endif
    }

    // Load the scene
    scene = load_scene("Room 1/Room 1.json");

    instance->scenes = scene;

    GXServer_t  *server         = load_server_as_json("G10/G10 server.json");
    instance->server = server;
    connect(server, "172.29.159.42", 8877);

    GXCommand_t *_join_command  = connect_command("Jake");
    GXCommand_t *_no_op_command = no_op_command();
    GXCommand_t *_chat_command  = chat_command(CHAT_ALL, "Hello, World!\n");
    
    //GXCommand_t *_displace_orient_command = displace_orient_command();

    enqueue_command(server, _join_command);
    enqueue_command(server, _no_op_command);
    enqueue_command(server, _chat_command);

    flush_commands(server);

    // Set up some binds
    {
        GXBind_t *quit = find_bind(instance->input, "QUIT");

        register_bind_callback(quit, &g_exit_game_loop);

        // Set up the camera controller from the primary camera.
        // NOTE: Requires the following binds: FORWARD, BACKWARD, STRAFE LEFT, STRAFE RIGHT, UP, DOWN, LEFT, and RIGHT. 
        camera_controller_from_camera(instance, scene->cameras);
        
    }

    // Splash screen animation
    {
        for (int i = 0; i > -15; i--)
        {
            move_front(i, i);
            render_textures();
            SDL_Delay(100);
        }
    }

    // Destroy the splash screen
    {
        #ifdef NDEBUG
            destroy_splashscreen();
        #endif
    }

    instance->running = true;

    g_window_resize(instance);

    // Main game loop
    while (instance->running)
    {
        // Compute delta time
        g_delta(instance);

        // Clear the screen
        g_clear();

        // Debug only FPS readout
        {
            #ifndef NDEBUG
                //printf("FPS: %.1f\r", (float)instance->deltaTime * (float)1000.f); // Uses CR instead of CR LF so as to provide a quasi realtime FPS readout
            #endif
        }

        // Process input
        process_input(instance);

        // Process networking actions
        {
            if (instance->server)
            {
                GXCommand_t* c = no_op_command();


                enqueue_command(server, c);

                flush_commands(server);
            }
        }

        // Compute physics
        if (instance->scenes)
            update_controlee_camera(instance->deltaTime);
            //compute_physics(scene, instance->deltaTime);

        // Draw the scene
        if(instance->scenes)
            draw_scene(instance->scenes);

        // Swap the window 
        g_swap(instance);
    }

    // G10 Unloading
    {
        g_exit(instance);
    }

    return 0;
}
