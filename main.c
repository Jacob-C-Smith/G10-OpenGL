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
#include <G10/G10.h>
#include <G10/GXtypedef.h>
#include <G10/GXScene.h>
#include <G10/GXCamera.h>
#include <G10/GXEntity.h>
#include <G10/GXRigidbody.h>
#include <G10/GXRig.h>
#include <G10/GXNetworking.h>
#include <G10/GXCollider.h>
#include <G10/GXPhysics.h>
#include <G10/GXSplashscreen.h>
#include <G10/GXHDR.h>


// G10 arch
#ifdef _M_X64
#include <G10/arch/x86_64/GXAVXmath.h>
#include <G10/arch/x86_64/GXAVXlinear.h>
#elif _M_ARM64
// ARM specific includes
#endif

#define MOUSE_SENS 0.05f

// For some reason, SDL defines main. I don't know why, but we have to undef it.
#undef main

int main( int argc, const char *argv[] )
{
    // Uninitialized data
   
    // Uninitialized G10 data
    GXScene_t    *scene;
    u32           d,
                  currentTime;
    
    // Uninitialized SDL data
    SDL_Window   *window;
    SDL_GLContext glContext;
    SDL_Event     event;

    // Initialized Data
    float         deltaTime    = 0.01f,
                  yaw          = 0.f,
                  pitch        = 0.f,
                  camSpeed     = 3.f;
    u8            running      = 1;
    u32	          lastTime     = 0;
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

    // G10 Initialization  
    {
        // Initialize SDL, GLAD, etc
        gInit(&window, &glContext);

        // Splash screen code
        #ifdef NDEBUG
            createSplashscreen("G10/splash front.png", "G10/splash back.png");
        #endif
        
        // Load the scene
        scene = loadScene(initialScene);
        
        // Splash screen animation
        {
            for (int i = 0; i > -15; i--)
            {
                moveFront(i, i);
                renderTextures();
                SDL_Delay(100);
            }
        }

        #ifdef NDEBUG
            destroySplashscreen();
        #endif

        SDL_ShowWindow(window);

    }
        
    goto setGLViewportSizeFromWindow;

    // Main game loop
    while (running)
    {
        // Calculate delta time
        {
            currentTime = SDL_GetTicks();
            d           = currentTime - lastTime;
            lastTime    = currentTime;
            deltaTime   = (float)1 / d;
        }

        // FPS readout
        {
            printf("FPS: %.1f\r", (float)deltaTime * (float)1000.f); // Uses CR instead of CR LF to provide a (kind of) realtime readout of the FPS
        }

        // TODO: Find a better way to process input.
        
        // Process input
        while (SDL_PollEvent(&event))  {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    running = 0;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    const u8* keyboardState = SDL_GetKeyboardState(NULL);

                    if (keyboardState[SDL_SCANCODE_G])
                    {
                        GXPart_t* part = getPart(scene->entities->parts, "hair");
                        part->transform->location.z += 1.f;
                    }
                    if(keyboardState[SDL_SCANCODE_CAPSLOCK])
                    {
                        // Toggle mouse lock 
                        SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                    }
                    
                    if (keyboardState[SDL_SCANCODE_U])
                    {
                        scene->cameras->where.x += 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_J])
                    {
                        scene->cameras->where.x -= 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_I])
                    {
                        scene->cameras->where.y += 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_K])
                    {
                        scene->cameras->where.y -= 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_O])
                    {
                        scene->cameras->where.z += 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_L])
                    {
                        scene->cameras->where.z -= 0.1f;
                    }
                    
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                        running = 0;

                    vec3 tw;
                    addVec3(&tw, scene->cameras->target, scene->cameras->where);

                    scene->cameras->view = lookAt(scene->cameras->where, tw, scene->cameras->up);
                }

                case SDL_KEYUP:
                {
                    //const u8* keyboardState = SDL_GetKeyboardState(NULL);

                    //updateCameraFromInput(scene->cameras, keyboardState, deltaTime);
                    vec3 tw;
                    addVec3(&tw, scene->cameras->target, scene->cameras->where);
                    scene->cameras->view = lookAt(scene->cameras->where, tw, scene->cameras->up);

                    break;
                }
                
                case SDL_MOUSEMOTION:
                {
                    if (SDL_GetRelativeMouseMode() == 0)
                        break;
                    GXCamera_t* a = scene->cameras;
                    
                    static float hAng = 0.f,
                                 vAng = 90.f;
                        
                    hAng += (float)event.motion.xrel * deltaTime * MOUSE_SENS * scene->cameras->fov / 90;
                    vAng += (float)event.motion.yrel * deltaTime * MOUSE_SENS * scene->cameras->fov / 90;
                        
                    if (vAng > (float)M_PI_2 - 0.0001f)
                        vAng = (float)M_PI_2 - 0.0001f;
                    if (vAng < (float)-M_PI_2 + 0.0001f)
                        vAng = (float)-M_PI_2 + 0.0001f;

                    a->target.x = sinf(hAng) * cosf(vAng);
                    a->target.y = cosf(hAng) * cosf(vAng);
                    a->target.z = sinf(-vAng);
                    
                    vec3 tw;
                    addVec3(&tw, a->target, a->where);

                    a->view = lookAt(a->where, tw, a->up);
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    // Up or down?
                    if (event.wheel.y < 0)
                        scene->cameras->fov += 1.f;
                    if (event.wheel.y > 0)
                        scene->cameras->fov -= 1.f;

                    // Clamp to 90 degrees
                    if (scene->cameras->fov >= 89.99f)
                        scene->cameras->fov = 89.99f;
                    if (scene->cameras->fov <= 1.f)
                        scene->cameras->fov = 1.f;
                        
                    computeProjectionMatrix(scene->cameras);
                    break;
                }
                    
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            // Respond to window resizing
                            setGLViewportSizeFromWindow:
                            {
                                // Initialized data
                                i32 w,
                                    h;

                                // Pull window data
                                SDL_GetWindowSize(window, &w, &h);
                                scene->cameras->fov = (float)w / h;

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

        const u8* keyboardState = SDL_GetKeyboardState(NULL);
        updateCameraFromInput(scene->cameras, keyboardState, deltaTime);

        // Clear the screen
        gClear();

        // G10
        {
            // Compute physics
            computePhysics(scene, deltaTime);

            // Draw the scene
            drawScene(scene);
        }

        // Swap the window 
        SDL_GL_SwapWindow(window);
    }


    // G10 Unloading
    {
        destroyScene(scene);
        gExit(window, glContext);
    }

    return 0;
}
