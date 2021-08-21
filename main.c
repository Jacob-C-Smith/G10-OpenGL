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

int main( int argc, const char* argv[] )
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
    char         *initialScene = 0;

    // Parse command line arguments
    {
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
                initialScene = argv[i + 1];
                continue;
            }
        }
    }

    // G10 Initialization  
    {
        // Initialize SDL, GLAD, etc
        gInit(&window, &glContext);

        // Splash screen code
        //#ifdef NDEBUG
            createSplashscreen("G10/splash.png");
        //#endif
        
        // Testing
        {

        }

        // Load the scene
        scene = loadScene(initialScene);
        
        //#ifdef NDEBUG
            destroySplashscreen();
        //#endif

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
            printf("FPS: %.1f\r", (float)deltaTime * 1000.f); // Uses CR instead of CR LF to provide a (kind of) realtime readout of the FPS
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
                    
                    if (keyboardState[SDL_SCANCODE_1])
                        ;

                    if (keyboardState[SDL_SCANCODE_W])
                    {
                        scene->cameras->where.x -= camSpeed * scene->cameras->view.c * deltaTime;
                        scene->cameras->where.y -= camSpeed * scene->cameras->view.g * deltaTime;
                        scene->cameras->where.z -= camSpeed * scene->cameras->view.k * deltaTime;
                    }
                    if (keyboardState[SDL_SCANCODE_A])
                    {
                        scene->cameras->where.x -= camSpeed * scene->cameras->view.a * deltaTime;
                        scene->cameras->where.y -= camSpeed * scene->cameras->view.e * deltaTime;
                        scene->cameras->where.z -= camSpeed * scene->cameras->view.i * deltaTime;
                    }
                    if (keyboardState[SDL_SCANCODE_S])
                    {
                        scene->cameras->where.x += camSpeed * scene->cameras->view.c * deltaTime;
                        scene->cameras->where.y += camSpeed * scene->cameras->view.g * deltaTime;
                        scene->cameras->where.z += camSpeed * scene->cameras->view.k * deltaTime;    
                    }
                    if (keyboardState[SDL_SCANCODE_D])
                    {
                        scene->cameras->where.x += camSpeed * scene->cameras->view.a * deltaTime;
                        scene->cameras->where.y += camSpeed * scene->cameras->view.e * deltaTime;
                        scene->cameras->where.z += camSpeed * scene->cameras->view.i * deltaTime;
                    }
                    if (keyboardState[SDL_SCANCODE_G])
                    {

                    }
                    if(keyboardState[SDL_SCANCODE_CAPSLOCK])
                    {
                        SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                    }
                    
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                        running = 0;

                    scene->cameras->view = lookAt(scene->cameras->where, addVec3(scene->cameras->target, scene->cameras->where), scene->cameras->up);
                }

                case SDL_KEYUP:
                {
                    break;
                }
                
                case SDL_MOUSEMOTION:
                {
                    if (SDL_GetRelativeMouseMode() == 0)
                        break;
                    GXCamera_t* a = scene->cameras;
                    
                    static float hAng = 0.f,
                                 vAng = 0.f;
                        
                    hAng += (float)event.motion.xrel * deltaTime * MOUSE_SENS * scene->cameras->fov * 1/90;
                    vAng += (float)event.motion.yrel * deltaTime * MOUSE_SENS * scene->cameras->fov * 1 / 90;
                        
                    if (vAng > M_PI_2 - 0.000001)
                        vAng = M_PI_2 - 0.000001;
                    if (vAng < -M_PI_2 + 0.000001)
                        vAng = -M_PI_2 + 0.000001;

                    a->target.x = sinf(hAng) * cosf(vAng);
                    a->target.y = cosf(hAng) * cosf(vAng);
                    a->target.z = sinf(-vAng);
                        
                    a->view = lookAt(a->where, addVec3(a->target, a->where), a->up);
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    if (event.wheel.y < 0)
                        scene->cameras->fov += 1.f;
                    if (event.wheel.y > 0)
                        scene->cameras->fov -= 1.f;
    
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
                            setGLViewportSizeFromWindow:
                            {
                                // Respond to window resizing
                                i32 w = 0, h = 0;

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

        // Clear the screen
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

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

    // G10 Unloading;
    {
        destroyScene(scene);
        gExit(window, glContext);
    }

    return 0;
}
