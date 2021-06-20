#include <G10/G10.h>

int gInit(SDL_Window** window, SDL_GLContext* glContext)
{
    // Argument Check
    {
        #ifndef NDEBUG
            if (window == (void*)0)
                goto nullWindow;
            if (glContext == (void*)0)
                goto nullGLContext;
        #endif
    }

    // Uninitialized data
    SDL_Window*         lWindow;
    SDL_GLContext       lGlContext;

    // SDL + GLAD Initialization
    {

        // Initialize SDL
        if (SDL_Init(SDL_INIT_EVERYTHING))
            goto noSDL;

        // Initialize the image loader library
        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
            goto noSDLImage;

        // Initialize the network library
        if (SDLNet_Init() == -1)
            goto noSDLNetwork;

        // Create the window
        lWindow = SDL_CreateWindow("G10",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ); 
        
        // Create the OpenGL context
        lGlContext = SDL_GL_CreateContext(lWindow);

        // Check the window
        if (!window)
            goto noWindow;

        // Check the OpenGL context
        if (!lGlContext)
            goto noSDLGLContext;

        // Check the glad loader
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
            goto gladFailed;

        // Enable VSync
        SDL_GL_SetSwapInterval(1);

        // Uncomment for mouse locking
        SDL_SetRelativeMouseMode(SDL_TRUE);

        // OpenGL Commands
        {
            // Enable depth testing and anti aliasing
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_MULTISAMPLE);
            glDepthFunc(GL_LESS);

            // TODO: Dynamically load active textures when needed for rendering
            // Initialize the active texture block
            /*
            {
                // Initialized data
                lActiveTextures = malloc(sizeof(GXActiveTextures_t));

                #ifndef NDEBUG
                    if (lActiveTextures == (void*)0)
                        return 0;
                #endif

                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &lActiveTextures->activeTextureCount);
                lActiveTextures->activeTextureBlock = calloc(lActiveTextures->activeTextureCount, sizeof(GXTexture_t));
                memset(lActiveTextures->activeTextureBlock, 0xFFFFFFFF, lActiveTextures->activeTextureCount * sizeof(GXTexture_t));
            }
            */

            // Set the clear color to white
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    *window    = lWindow;
    *glContext = lGlContext;

    return 0;

    // Error handling
    {
        // The provided window double pointer was null
        nullWindow:
        #ifndef NDEBUG
            printf("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // The provided OpenGL context pointer was null
        nullGLContext:
        #ifndef NDEBUG
            printf("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // SDL failed to initialize
        noSDL:
        #ifndef NDEBUG
            printf("[SDL2] Failed to initialize SDL\n");
        #endif
        return 0;

        // SDL Image failed to initialize
        noSDLImage:
        #ifndef NDEBUG
            printf("[SDL Image] Failed to initialize SDL Image\n");
        #endif
        return 0;

        // SDL Networking failed to initialize
        noSDLNetwork:
        #ifndef NDEBUG
            printf("[SDL Networking] Failed to initialize SDL Networking\n");
        #endif
        return 0;

        // The SDL window failed to initialize
        noWindow:
        #ifndef NDEBUG
            printf("[SDL2] Failed to create a window\nSDL Says: %s\n", SDL_GetError());
        #endif
        return 0;

        // The OpenGL context failed to initialize
        noSDLGLContext:
        #ifndef NDEBUG
            printf("[SDL2] Failed to create an OpenGL Context\nSDL Says: %s", SDL_GetError());
        #endif
        return 0;

        // The OpenGL loader failed
        gladFailed:
        #ifndef NDEBUG
            printf("[GLAD] Failed to load OpenGL\n");
        #endif 
        return 0;
    }
}

int gExit(SDL_Window* window, SDL_GLContext  glContext)
{
    // SDL Deinitialization
    {
        SDL_DestroyWindow(window);
        SDL_GL_DeleteContext(glContext);
        SDLNet_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    return 0;
}