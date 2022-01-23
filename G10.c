#include <G10/G10.h>

GXInstance_t *g_init         ( const char *path )
{
    // Argument Check
    {
        #ifndef NDEBUG
            if (path == (void*)0)
                goto nullPath;
        #endif
    }

    // Initialized data
    GXInstance_t *ret = calloc(1, sizeof(GXInstance_t));

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
        ret->window = SDL_CreateWindow("G10",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1600, 900,
            SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ); 
        
        // Context attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Create the OpenGL context
        ret->glContext = SDL_GL_CreateContext(ret->window);

        // Check the window
        if (!ret->window)
            goto noWindow;

        // Display the window
        SDL_ShowWindow(ret->window);

        // Check the OpenGL context
        if (!ret->glContext)
            goto noSDLGLContext;

        // Check the glad loader
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
            goto gladFailed;

        #ifndef NDEBUG
            if (ret->glContext == (void*)0)
                goto nullGLContext;
        #endif

        // Enable VSync
        SDL_GL_SetSwapInterval(1);

        // Uncomment for mouse locking
        SDL_SetRelativeMouseMode(SDL_TRUE);

        // OpenGL Commands
        {
            const u8* vendor          = glGetString(GL_VENDOR);
            const u8* renderer        = glGetString(GL_RENDERER);
            size_t    extensionsCount = 0;
            glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);

            // Log every extension
            #ifndef NDEBUG
                g_print_log("[G10] Loaded OpenGL driver from \"%s\" with hint \"%s\"\n", vendor, renderer);
                g_print_log("[G10] %d OpenGL extensions are available\n", extensionsCount);
                for (size_t i = 0; i < extensionsCount; i++)
                    g_print_log("\t - %s\n",glGetStringi(GL_EXTENSIONS, i));
            #endif

            // Enable depth testing, anti aliasing, and some other stuff
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            glEnable(GL_MULTISAMPLE);
            glLineWidth(3);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // Initialize the active texture block
            {
                // Uninitialized data
                extern GXTextureUnit_t *activeTextures;

                activeTextures = calloc(1,sizeof(GXTextureUnit_t));

                #ifndef NDEBUG
                    if (activeTextures == (void*)0)
                        return 0;
                #endif

                // Find out how many texture units the card has
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &activeTextures->active_texture_count);

                // Allocate the space for them
                activeTextures->active_texture_block = calloc(activeTextures->active_texture_count, sizeof(GXTexture_t*));

                // Print the texture count if running in debug mode
                #ifndef NDEBUG
                    g_print_log("[G10] [Texture manager] Texture manager initialized with %d active textures\n", activeTextures->active_texture_count);
                #endif
            }

            // Set the clear color to white
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Load a missing texture texture
        {
            extern GXTexture_t* missingTexture;
            missingTexture = load_texture("G10/missing texture.png");
        }
    }

    ret->deltaTime = 0.01f;

    return ret;

    // Error handling
    {
        // The provided window double pointer was null
        nullPath:
        #ifndef NDEBUG
            g_print_error("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // The provided OpenGL context pointer was null
        nullGLContext:
        #ifndef NDEBUG
            g_print_error("[G10] Null pointer provided to %s.\n", __FUNCSIG__);
        #endif
        return 0;

        // SDL failed to initialize
        noSDL:
        #ifndef NDEBUG
            g_print_error("[SDL2] Failed to initialize SDL\n");
        #endif
        return 0;

        // SDL Image failed to initialize
        noSDLImage:
        #ifndef NDEBUG
            g_print_error("[SDL Image] Failed to initialize SDL Image\n");
        #endif
        return 0;

        // SDL Networking failed to initialize
        noSDLNetwork:
        #ifndef NDEBUG
            g_print_error("[SDL Networking] Failed to initialize SDL Networking\n");
        #endif
        return 0;

        // The SDL window failed to initialize
        noWindow:
        #ifndef NDEBUG
            g_print_error("[SDL2] Failed to create a window\nSDL Says: %s\n", SDL_GetError());
        #endif
        return 0;

        // The OpenGL context failed to initialize
        noSDLGLContext:
        #ifndef NDEBUG
            g_print_error("[SDL2] Failed to create an OpenGL Context\nSDL Says: %s", SDL_GetError());
        #endif
        return 0;

        // The OpenGL loader failed
        gladFailed:
        #ifndef NDEBUG
            g_print_error("[GLAD] Failed to load OpenGL\n");
        #endif 
        return 0;
    }
}

size_t        g_load_file     ( const char       *path,   void          *buffer   , bool binaryMode )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if(path==0)
                goto noPath;
        #endif
    }

    // Initialized data
    size_t  ret = 0;
    FILE   *f   = fopen(path, (binaryMode) ? "rb" : "r");
    
    // Check if file is valid
    if (f == NULL)
        goto invalidFile;

    // Find file size and prep for read
    fseek(f, 0, SEEK_END);
    ret = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Read to data
    if(buffer)
        ret = fread(buffer, 1, ret, f);

    // We no longer need the file
    fclose(f);
    
    return ret;

    // Error handling
    {
        noPath:
        #ifndef NDEBUG
            g_print_error("[G10] Null path provided to funciton \"%s\\n",__FUNCSIG__);
        #endif
        return 0;
        
        invalidFile:
        #ifndef NDEBUG
            g_print_error("[G10] Failed to load file \"%s\"\n",path);
        #endif
        return 0;
    }
}

int           g_print_error   ( const char *const format, ... ) 
{
    // TODO: Argument check
    
    // We use the varadic argument list in vprintf
    va_list aList;
    va_start(aList, format);

    // Uses ANSI terminal escapes to set the color to red, 
    // print the message, and restore the color.
    printf("\033[91m");
    vprintf(format, aList);
    printf("\033[0m");

    va_end(aList);

    return 0;
    // TODO: Error handling
}

int           g_print_warning ( const char *const format, ... ) 
{
    // TODO: Argument check

    // We use the varadic argument list in vprintf
    va_list aList;
    va_start(aList, format);

    // Uses ANSI terminal escapes to set the color to yellow, 
    // print the message, and restore the color.
    printf("\033[93m");
    vprintf(format, aList);
    printf("\033[0m");

    va_end(aList);

    return 0;
    // TODO: Error handling

}

int           g_print_log     ( const char *const format, ... ) 
{
    // TODO: Argument check

    // We use the varadic argument list in vprintf
    va_list aList;
    va_start(aList, format);

    // Uses ANSI terminal escapes to set the color to light blue, 
    // print the message, and restore the color.
    printf("\033[94m");
    vprintf(format, aList);
    printf("\033[0m");

    va_end(aList);
    va_end(aList);

    return 0;
    // TODO: Error handling

}

int           g_clear         ( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int g_swap(GXInstance_t* instance)
{
    // TODO: Argument checking
    SDL_GL_SwapWindow(instance->window);
    return 0;
    // TODO: Error handling

}

int g_delta(GXInstance_t* instance)
{
    // TODO: Argument checking
    // Calculate delta time
    instance->currentTime = SDL_GetTicks();
    instance->d = instance->currentTime - instance->lastTime;
    instance->lastTime = instance->currentTime;
    instance->deltaTime = (float)1 / instance->d;

    return 0;
    // TODO: Error handling

}

u8            g_checksum     ( u8               *data  , size_t         len)
{
    u8 ret = 0;
    for (size_t i = 0; i < len; i++)
        ret += data[i];

    return ~ret;
}

int           g_exit         ( GXInstance_t *instance )
{
    // SDL Deinitialization
    {
        SDL_DestroyWindow(instance->window);
        SDL_GL_DeleteContext(instance->glContext);
        SDLNet_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    return 0;
}