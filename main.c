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
    // Uninitialized data
    u32           d,
                  currentTime;
    clock_t       c;

    GXPart_t     *cube;
	GXShader_t   *shader;
	
    // Uninitialized SDL data
    SDL_Window   *window;
    SDL_GLContext glContext;
    SDL_Event     event;

    // Initialized Data

    // Initialized G10 data
    GXScene_t    *scene        = 0;
    GXServer_t   *server       = 0;
    
    bool          drawBVH      = false;

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

        // Splash screen
        {
            #ifdef NDEBUG
                createSplashscreen("G10/splash/splash front.png", "G10/splash/splash back.png");
            #endif
        }

        // Load the scene
        scene = loadScene(initialScene);

        {
            printBV(scene->BVH, 0);
            
            // Load a cube and a solid color shader so we can draw bounding volumes
            cube   = loadPart("G10/cube.json");
            shader = loadShader("G10/shaders/G10 solid color.json");
        }

        // Splash screen animation
        {
            for (int i = 0; i > -15; i--)
            {
                moveFront(i, i);
                renderTextures();
                SDL_Delay(100);
            }
        }

        // Destroy the splash screen
        {
            #ifdef NDEBUG
                destroySplashscreen();
            #endif
        }

        // Display the window
        SDL_ShowWindow(window);

    }

    // G10 Testing
    {

    }

    // Server testing
    /*
    {
        server = createServer();
        connect(server, " 172.27.162.87 ", 8877);
        char* party[] = { "Seth", "Daniel", "Elias" };
        
        char* name = calloc(255+1, sizeof(u8));

        sendConnectCommand(server, "Jake", party);
        sendTextChat(server, "Hello, World!", 0x0F); 
    }*/

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
            #ifndef NDEBUG
                printf("FPS: %.1f\r", (float)deltaTime * (float)1000.f); // Uses CR instead of CR LF to provide a (kind of) realtime readout of the FPS
            #endif
        }

        // TODO: Find a better way to process input.
        
        // Process input
        {
            // Process events
            while (SDL_PollEvent(&event)) {
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
                    static float orientation  = 0.f;
                    static float lOrientation = 0.f;
                    if (keyboardState[SDL_SCANCODE_G])
                    {
                        orientation += 0.1f;
                    }
                    if (keyboardState[SDL_SCANCODE_CAPSLOCK])
                    {
                        // Toggle mouse lock 
                        SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                    }

                    
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
                    

                    GXEntity_t* tank = 0;
                    tank = getEntity(scene, "Tank");
                    GXTransform_t* transform = tank->transform;

                    if (tank == 0)
                        return 0;
                    if (transform == 0)
                        return 0;

                    tank->transform->location.x += cosf(orientation ) * deltaTime,
                    tank->transform->location.y += sinf(orientation ) * deltaTime;

                    lOrientation = orientation;
                    
                    printf("%g\n", orientation);

                    

                    noInput:

                    //sendDisplaceOrientCommand(server, tank);

                    if (keyboardState[SDL_SCANCODE_ESCAPE])
                        running = 0;

                    // Turn on BVH drawings
                    if (keyboardState[SDL_SCANCODE_F1])
                        drawBVH = !drawBVH;

                    vec3 tw;
                    addVec3(&tw, scene->cameras->target, scene->cameras->where);

                    scene->cameras->view = lookAt(scene->cameras->where, tw, scene->cameras->up);
                }
                case SDL_KEYUP:
                {
                    //const u8* keyboardState = SDL_GetKeyboardState(NULL);

                    //updateCameraFromKeyboardInput(scene->cameras, keyboardState, deltaTime);
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
                            int w,
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

            // Update camera location
            const u8* keyboardState = SDL_GetKeyboardState(NULL);
            updateCameraFromKeyboardInput(scene->cameras, keyboardState, deltaTime);
        }

        // Get response from server
        {
            //char *buffer = calloc(4096,1);
            //recvCommand(server, buffer, 4096);       
        }

        // Clear the screen
        gClear();

        // G10
        {
            // Compute physics
            computePhysics(scene, deltaTime);

            // Draw the scene
            drawScene(scene);

            // Draw bounding volumes, if we are supposed to
            if(drawBVH)
                drawSceneBV(scene, cube, shader, 0);
        }

        // Swap the window 
        SDL_GL_SwapWindow(window);
    }

    // G10 Unloading
    {
        if(server)
            sendDisconnectCommand(server);
        destroyScene(scene);
        gExit(window, glContext);
    }

    return 0;
}
