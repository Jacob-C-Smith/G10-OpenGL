#include <G10/G10.h>
HANDLE hConsole;

int gInit( SDL_Window **window, SDL_GLContext *glContext )
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
    SDL_Window         *lWindow;
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
            1600, 900,
            SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ); 
        
        // Context attributes

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

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

            // Initialize the active texture block
            {
                // Uninitialized data
                extern GXTextureUnit_t *activeTextures;

                activeTextures = malloc(sizeof(GXTextureUnit_t));

                #ifndef NDEBUG
                    if (activeTextures == (void*)0)
                        return 0;
                #endif

                // Find out how many texture units the card has
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &activeTextures->activeTextureCount);

                // Allocate the space for them
                activeTextures->activeTextureBlock = calloc(activeTextures->activeTextureCount, sizeof(GXTexture_t*));
            }

            // Set the clear color to white
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    // Windows console initialization
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    *window    = lWindow;
    *glContext = lGlContext;

    return 0;

    // Error handling
    {
        // The provided window double pointer was null
        nullWindow:
        #ifndef NDEBUG
            gPrintError("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // The provided OpenGL context pointer was null
        nullGLContext:
        #ifndef NDEBUG
            gPrintError("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // SDL failed to initialize
        noSDL:
        #ifndef NDEBUG
            gPrintError("[SDL2] Failed to initialize SDL\n");
        #endif
        return 0;

        // SDL Image failed to initialize
        noSDLImage:
        #ifndef NDEBUG
            gPrintError("[SDL Image] Failed to initialize SDL Image\n");
        #endif
        return 0;

        // SDL Networking failed to initialize
        noSDLNetwork:
        #ifndef NDEBUG
            gPrintError("[SDL Networking] Failed to initialize SDL Networking\n");
        #endif
        return 0;

        // The SDL window failed to initialize
        noWindow:
        #ifndef NDEBUG
            gPrintError("[SDL2] Failed to create a window\nSDL Says: %s\n", SDL_GetError());
        #endif
        return 0;

        // The OpenGL context failed to initialize
        noSDLGLContext:
        #ifndef NDEBUG
            gPrintError("[SDL2] Failed to create an OpenGL Context\nSDL Says: %s", SDL_GetError());
        #endif
        return 0;

        // The OpenGL loader failed
        gladFailed:
        #ifndef NDEBUG
            gPrintError("[GLAD] Failed to load OpenGL\n");
        #endif 
        return 0;
    }
}

int gPrintError( const char* const format, ... )
{
    va_list aList;
    va_start(aList, format);

    SetConsoleTextAttribute(hConsole, 0x0C);
    vprintf(format, aList);
    SetConsoleTextAttribute(hConsole, 0x0F);

    va_end(aList);

    return 0;
}

int gPrintWarning(const char* const format, ...)
{
    va_list aList;
    va_start(aList, format);

    SetConsoleTextAttribute(hConsole, 0x0E);
    vprintf(format, aList);
    SetConsoleTextAttribute(hConsole, 0x0F);

    va_end(aList);

    return 0;
}

int gPrintLog ( const char* const format, ... )
{
    va_list aList;
    va_start(aList, format);

    SetConsoleTextAttribute(hConsole, 0x0B);
    vprintf(format, aList);
    SetConsoleTextAttribute(hConsole, 0x0F);

    va_end(aList);

    return 0;
}

int gExit( SDL_Window* window, SDL_GLContext  glContext )
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