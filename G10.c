#include <G10/G10.h>

static GXInstance_t *active_instance = 0;

GXInstance_t *g_init              ( const char       *path )
{
    // Argument Check
    {
        #ifndef NDEBUG
            if (path == (void*)0)
                goto nullPath;
        #endif
    }

    // Initialized data
    GXInstance_t *ret            = calloc(1, sizeof(GXInstance_t));
    size_t        i              = g_load_file(path, 0, false),
                  token_count    = 0;
    JSONToken_t  *tokens         = 0;
    char         *token_text     = calloc(i, sizeof(u8)),
                 *window_title   = "G10";
    i32           window_width   = 640,
                  window_height  = 480;
    bool          fullscreen     = false;
    size_t        token_text_len = 0;
    
    // Load the file
    g_load_file(path, token_text, false);

    token_text_len = strlen(token_text);

    // Parse the JSON
    token_count = parse_json(token_text, token_text_len, 0, 0);
    tokens      = calloc(token_count, sizeof(JSONToken_t));
    parse_json(token_text, token_text_len, token_count, tokens);

    for (i = 0; i < token_count; i++)
    {
        if (strcmp(tokens[i].key, "name")          == 0)
        { 
            if (tokens[i].type == JSONstring)
            {
                char* n = tokens[i].value.n_where;
                size_t l = strlen(n);
                ret->name = calloc(l + 1, sizeof(char));

                strncpy(ret->name, n, l);
            }
            continue;
        }
        if (strcmp(tokens[i].key, "window width")  == 0)
        { 
            if (tokens[i].type == JSONprimative)
                window_width = atoi(tokens[i].value.n_where);
            continue;
        }
        if (strcmp(tokens[i].key, "window height") == 0)
        { 
            if (tokens[i].type == JSONprimative)
                window_height = atoi(tokens[i].value.n_where);
            continue;
        }
        if (strcmp(tokens[i].key, "window title")  == 0)
        { 
            if (tokens[i].type == JSONstring)
            {
                char* n = tokens[i].value.n_where;
                size_t l = strlen(n);
                window_title = calloc(l + 1, sizeof(char));

                strncpy(window_title, n, l);
            }
            continue;
        }
        if (strcmp(tokens[i].key, "fullscreen")    == 0)
        {
            if (tokens[i].type == JSONprimative)
                fullscreen = (bool)tokens[i].value.n_where;
            continue;
       }
        if (strcmp(tokens[i].key, "input")         == 0)
        {
            if (tokens[i].type == JSONstring)
                ret->input = load_input(tokens[i].value.n_where);
            else
                ret->input = load_input_as_json(tokens[i].value.n_where);

            continue;
        }
    }

    free(tokens);
    free(token_text);

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
        ret->window = SDL_CreateWindow(window_title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width, window_height,
            SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | ((fullscreen==1) ? SDL_WINDOW_RESIZABLE : 0) ); 

        // Context attributes
        // OpenGL 4.6
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

        // 4x Multisampling
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
            glLineWidth(2);
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

            // Set the clear color to black
            glClearColor(1.f, 1.f, 1.f, 1.0f);
        }

        // Load a missing texture texture
        {
            extern GXTexture_t* missingTexture;
            missingTexture = load_texture("G10/missing texture.png");
        }
    }

    free(window_title);

    ret->delta_time = 0.01f;

    ret->cached_entities       = calloc(128, sizeof(void*));
    ret->cached_parts          = calloc(128, sizeof(void*));
    ret->cached_materials      = calloc(128, sizeof(void*));
    ret->cached_textures       = calloc(128, sizeof(void*));

    ret->cached_entity_names   = calloc(128, sizeof(void*));
    ret->cached_part_names     = calloc(128, sizeof(void*));
    ret->cached_material_names = calloc(128, sizeof(void*));
    ret->cached_texture_names  = calloc(128, sizeof(void*));


    active_instance = ret;

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

size_t        g_load_file         ( const char       *path,   void          *buffer   , bool binaryMode )
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

    // The file is no longer needed
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

int           g_print_error       ( const char *const format, ... ) 
{
    // Argument check
    {
        if (format == (void*)0)
            goto no_format;
    }

    // Use the varadic argument list in vprintf call
    va_list aList;
    va_start(aList, format);

    // Uses ANSI terminal escapes to set the color to red, 
    // print the message, and restore the color.
    printf("\033[91m");

    // TODO: Configure error messages to write to other files?
    vfprintf(stdout, format, aList);
    printf("\033[0m");

    va_end(aList);

    return 0;
    
    // Error handling
    {
        no_format:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"format\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
    }
}

int           g_print_warning     ( const char *const format, ... ) 
{
    // Argument check
    {
        if (format == (void*)0)
            goto no_format;
    }

    // Use the varadic argument list in vprintf call
    va_list aList;
    va_start(aList, format);

    // Uses ANSI terminal escapes to set the color to yellow, 
    // print the message, and restore the color.
    printf("\033[93m");
    vprintf(format, aList);
    printf("\033[0m");

    va_end(aList);

    return 0;
    // Error handling
    {
        no_format:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"format\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
    }

}

int           g_print_log         ( const char *const format, ... ) 
{
    // Argument check
    {
        if (format == (void*)0)
            goto no_format;
    }

    // Use the varadic argument list in vprintf call
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
    // Error handling
    {
        no_format:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"format\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
    }

}

int           g_clear             ( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int           g_swap              ( GXInstance_t     *instance )
{

    // Argument checking
    {
        if(instance == (void *)0)
            goto noInstance;

        if (instance->window == (void*)0)
            goto noWindow;
    }

    SDL_GL_SwapWindow(instance->window);
    
    return 0;
    
    // Error handling
    {
        noInstance:
        #ifndef NDEBUG
            g_print_error("[G10] Null pointer provided for \"instance\" in call to function \"%s\"\n"__FUNCSIG__);
        #endif
        return 0;

        noWindow:
        #ifndef NDEBUG
            g_print_error("[G10] Null pointer for \"window\" in instance in call to function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }


}

int           g_window_resize     ( GXInstance_t     *instance )
{
    // Uninitialized data
    int w,
        h;

    // Pull window data
    SDL_GetWindowSize(instance->window, &w, &h);

    // Notify OpenGL of the change
    glViewport(0, 0, w, h);
}

int           g_exit_game_loop    ( callback_parameter_t  c, GXInstance_t* i )
{
    if(c.input_state == KEYBOARD)
        if(c.inputs.key.depressed == true)
            i->running = false;

    return 0;
}

GXInstance_t* g_get_active_instance(void)
{
    return active_instance;
}

int g_cache_material(GXInstance_t* instance, GXMaterial_t* material)
{
    if (instance->cached_material_count >= 128)
        return 0;

    instance->cached_material_count++;

    for (size_t i = 0; i < instance->cached_material_count; i++)
    {
        if (instance->cached_materials[i])
            continue;

        instance->cached_materials[i] = material,
            instance->cached_material_names[i] = material->name;

        break;
    }

    return 0;
}

int g_cache_part ( GXInstance_t *instance, GXPart_t *part )
{
    if (instance->cached_part_count >= 128)
        return 0;

    instance->cached_part_count++;

    for (size_t i = 0; i < instance->cached_part_count; i++)
    {
        if (instance->cached_parts[i])
            continue;

        instance->cached_parts[i] = part,
        instance->cached_part_names[i] = part->name;

        break;
    }

    return 0;
}


GXMaterial_t* g_find_material(GXInstance_t* instance, char* name)
{

    // Argument check
    {
        if (instance == (void*)0)
            goto no_instance;
        if (name == (void*)0)
            goto no_name;
    }

    // Search the cache for materials
    for (size_t i = 0; i < instance->cached_material_count; i++)
        if (strcmp(instance->cached_material_names[i], name) == 0)
            return instance->cached_materials[i];

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_instance:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"instance\" in call to function \"%s\"\n", __FUNCSIG__ );
            #endif
            return 0;

            no_name:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"name\" in call to function \"%s\"\n", __FUNCSIG__ );
            #endif
            return 0;
        }
    }
}

GXPart_t* g_find_part(GXInstance_t* instance, char* name)
{
    // Argument check
    {
        if (instance == (void*)0)
            goto no_instance;
        if (name == (void*)0)
            goto no_name;
    }

    // Search the cache for parts
    for (size_t i = 0; i < instance->cached_part_count; i++)
        if (strcmp(instance->cached_part_names[i], name) == 0)
            return instance->cached_parts[i];

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_instance:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"instance\" in call to function \"%s\"\n", __FUNCSIG__ );
            #endif
            return 0;

            no_name:
            #ifndef NDEBUG
                g_print_error("[G10] Null pointer provided for \"name\" in call to function \"%s\"\n", __FUNCSIG__ );
            #endif
            return 0;
        }
    }
}

int           g_delta             ( GXInstance_t     *instance )
{
    // Argument checking
    {
        if (instance == (void*)0)
            goto noInstance;
    }

    // Calculate delta time
    instance->ticks       = SDL_GetTicks();
    instance->d           = instance->ticks - instance->lastTime;
    instance->lastTime    = instance->ticks;
    instance->delta_time   = (float)1 / instance->d;

    return 0;
    
    // Error handling
    {
        noInstance:
        #ifndef NDEBUG
            g_print_error("[G10] Null pointer provided for \"instance\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }

}

void          g_toggle_mouse_lock ( void )
{
    SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
}

int           g_exit              ( GXInstance_t     *instance )
{
    //if(instance->server)

    // G10 Deinitialization
    if(instance->scenes)
    {
        GXScene_t* i = instance->scenes;

        // Destroy all scenes in the instance
        while (i)
        {
            GXScene_t* j = i;
            i = i->next;
            destroy_scene(j);
        }

    }

    if(instance->input)
        destroy_input(instance->input);

    extern GXTextureUnit_t *activeTextures;
    extern GXTexture_t     *missingTexture;
    unload_texture(missingTexture);
    free(activeTextures->active_texture_block);
    free(activeTextures);
    free(instance->name);

    // SDL Deinitialization
    {
        SDL_DestroyWindow(instance->window);
        SDL_GL_DeleteContext(instance->glContext);
        SDLNet_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    
    free(instance);
    
    return 0;
}