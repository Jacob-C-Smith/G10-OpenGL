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
#include <G10/GXInput.h>
#include <G10/GXCamera.h>
#include <G10/GXEntity.h>
#include <G10/GXRigidbody.h>
#include <G10/GXRig.h>
#include <G10/GXNetworking.h>
#include <G10/GXCollider.h>
#include <G10/GXPhysics.h>
#include <G10/GXSplashscreen.h>
#include <G10/GXHDR.h>
#include <G10/GXBVH.h>

// G10 arch
#ifdef _M_X64
// x86_64
#include <G10/arch/x86_64/GXAVXmath.h>
#include <G10/arch/x86_64/GXAVXlinear.h>
#elif _M_ARM64
// ARM specific includes
#endif

#define MOUSE_SENS 0.05f

// For some reason, SDL defines main. I don't know why, but we have to undef it.
#undef main

int main ( int argc, const char *argv[] )
{
    // Initialized G10 data
    GXInstance_t *instance     = 0;
    GXScene_t    *scene        = 0;
    GXServer_t   *server       = 0;
    GXInput_t    *input        = load_input("G10/input.json");

    bool          drawBVH      = false;

    float         yaw          = 0.f,
                  pitch        = 0.f,
                  camSpeed     = 300.f;
    const char   *initialScene = 0,
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
            instance = g_init("G10/debug instance.json");
        #else 
            instance = g_init("G10/release inscance.json");
        #endif
    }

    // Create a splash screen
    {
        #ifdef NDEBUG
            create_splashscreen("G10/splash/splash front.png", "G10/splash/splash back.png");
        #endif
    }

    // Load the scene
    scene = load_scene("Nitric Acid Plant/Nitric Acid Plant.json");

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

    goto setGLViewportSizeFromWindow;

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
                printf("FPS: %.1f\r", (float)instance->deltaTime * (float)1000.f); // Uses CR instead of CR LF to provide a (kind of) realtime readout of the FPS
            #endif
        }

        // TODO: Finish callback events
        // Process input
        {
            // Process events
            while (SDL_PollEvent(&instance->event)) {
                switch (instance->event.type)
                {
                case SDL_QUIT:
                {
                    instance->running = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    const  u8   *keyboardState = SDL_GetKeyboardState(NULL);
                    static float orientation   = 0.f;
                    static float lOrientation  = 0.f;
                    static vec3  offset        = { 0.f, 0.f, 0.f };

                    if (keyboardState[SDL_SCANCODE_F1])
                    {
                        GXScene_t *lScene = scene;
                        scene = 0;
                        scene = load_scene("Nitric Acid Plant/Nitric Acid Plant.json");
                        destroy_scene(lScene);
                    }

                    if (keyboardState[SDL_SCANCODE_KP_8])
                        scene->cameras->where.z += 0.1f;
                    if (keyboardState[SDL_SCANCODE_KP_2])
                        scene->cameras->where.z -= 0.1f;

                    if (keyboardState[SDL_SCANCODE_U])
                        offset.x += 0.05f;
                    if (keyboardState[SDL_SCANCODE_I])
                        offset.y += 0.05f;
                    if (keyboardState[SDL_SCANCODE_O])
                        offset.z += 0.05f;
                    


                    if (keyboardState[SDL_SCANCODE_CAPSLOCK])
                        // Toggle mouse lock 
                        SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());

                    
                    // Turn keyboard input into orientation 
                    {
                        if (keyboardState[SDL_SCANCODE_I])
                            if (keyboardState[SDL_SCANCODE_J])
                                orientation = (float) (M_PI - M_PI_4);         // ↖ IJ, 135°, 3π/4 rad
                            else if (keyboardState[SDL_SCANCODE_L])
                                orientation = (float)(M_PI_4);                 // ↗ IL, 45° , π/4  rad
                            else
                                orientation = (float)(M_PI_2);                 // ↑ I,  90° , π/2  rad
                        else if (keyboardState[SDL_SCANCODE_J])
                            if (keyboardState[SDL_SCANCODE_K])
                                orientation = (float)(M_PI + M_PI_4);          // ↙ JK, 225°, 5π/4 rad
                            else
                                orientation = (float)(M_PI);                   // ← J,  180°, π    rad
                        else if (keyboardState[SDL_SCANCODE_L])
                            if (keyboardState[SDL_SCANCODE_K])
                                orientation = (float)(M_PI + M_PI_2 + M_PI_4); // ↘ LK, 315°, 7π/4 rad
                            else
                                orientation = 0.f;                             // → L,  0°  , 0    rad
                        else if (keyboardState[SDL_SCANCODE_K])
                            orientation = (float)(M_PI + M_PI_2);              // ↓ K, 270° , 3π/2 rad
                        else
                            goto noInput;                                      // / (None)
                    }
                    
                    noInput:

                    //sendDisplaceOrientCommand(server, tank);

                    if (keyboardState[SDL_SCANCODE_ESCAPE])
                        instance->running = 0;

                    // Turn on BVH drawings
                    if (keyboardState[SDL_SCANCODE_F1])
                        drawBVH = !drawBVH;

                    vec3 tw;
                    add_vec3(&tw, scene->cameras->target, scene->cameras->where);

                    scene->cameras->view = look_at(scene->cameras->where, tw, scene->cameras->up);
                }
                case SDL_KEYUP:
                {
                    vec3 tw;
                    add_vec3(&tw, scene->cameras->target, scene->cameras->where);
                    scene->cameras->view = look_at(scene->cameras->where, tw, scene->cameras->up);

                    break;
                }
                case SDL_MOUSEMOTION:
                {


                    if (SDL_GetRelativeMouseMode() == 0)
                        break;

                    GXCamera_t* a = scene->cameras;

                    static float hAng = 90.f,
                                 vAng = 90.f;

                    hAng += (float)instance->event.motion.xrel * instance->deltaTime * MOUSE_SENS * scene->cameras->fov / 90;
                    vAng += (float)instance->event.motion.yrel * instance->deltaTime * MOUSE_SENS * scene->cameras->fov / 90;

                    if (vAng > (float)M_PI_2 - 0.0001f)
                        vAng = (float)M_PI_2 - 0.0001f;
                    if (vAng < (float)-M_PI_2 + 0.0001f)
                        vAng = (float)-M_PI_2 + 0.0001f;

                    a->target.x = sinf(hAng) * cosf(vAng);
                    a->target.y = cosf(hAng) * cosf(vAng);
                    a->target.z = sinf(-vAng);

                    vec3 tw;
                    add_vec3(&tw, a->target, a->where);

                    a->view = look_at(a->where, tw, a->up);
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    // Up or down?
                    if (instance->event.wheel.y < 0)
                        scene->cameras->fov += 1.f;
                    if (instance->event.wheel.y > 0)
                        scene->cameras->fov -= 1.f;

                    // Clamp to 90 degrees
                    if (scene->cameras->fov >= 89.98f)
                        scene->cameras->fov = 89.98f;
                    if (scene->cameras->fov <= 1.f)
                        scene->cameras->fov = 1.f;

                    computeProjectionMatrix(scene->cameras);
                    break;
                }
                case SDL_WINDOWEVENT:
                    switch (instance->event.window.event)
                    {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        // Respond to window resizing
                        setGLViewportSizeFromWindow:
                        {
                            // Initialized data
                            int w,
                                h;

                            // Pull window data
                            SDL_GetWindowSize(instance->window, &w, &h);
                            scene->cameras->aspect_ratio = (float)w / h;

                            // Notify OpenGL of the change
                            glViewport(0, 0, w, h);
                        }
                        break;
                    }
                    default:
                        break;
                    }
                default:
                    break;
                }
            }

            // Update camera location
            const u8* keyboardState = SDL_GetKeyboardState(NULL);
            update_camera_from_keyboard_input(scene->cameras, keyboardState, instance->deltaTime);
        }

        // G10
        {
            // Compute physics
            if(scene)
                //compute_physics(scene, deltaTime);

            // Draw the scene
            if(scene)
                draw_scene(scene);

            // Post drawing

        }

        // Swap the window 
        g_swap(instance);
    }

    // G10 Unloading
    {
        if(server)
            destroy_server(server);
        if(scene)
            destroy_scene(scene);

        g_exit(instance);
    }

    return 0;
}
