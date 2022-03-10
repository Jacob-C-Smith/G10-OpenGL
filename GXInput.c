#include <G10/GXInput.h>

typedef struct { SDL_KeyCode code; const char* name; } kn_t;
const kn_t keys[] = {
    {
        SDL_SCANCODE_RETURN,
        "RETURN"
    },
    {
        SDL_SCANCODE_ESCAPE,
        "ESCAPE"
    },
    {
        SDL_SCANCODE_BACKSPACE,
        "BACKSPACE"
    },
    {
        SDL_SCANCODE_TAB,
        "TAB"
    },
    {
        SDL_SCANCODE_SPACE,
        "SPACE"
    },
    {
        SDL_SCANCODE_COMMA,
        "COMMA"
    },
    {
        SDL_SCANCODE_MINUS,
        "MINUS"
    },
    {
        SDL_SCANCODE_PERIOD,
        "PERIOD"
    },
    {
        SDL_SCANCODE_SLASH,
        "SLASH"
    },
    {
        SDL_SCANCODE_0,
        "0"
    },
    {
        SDL_SCANCODE_1,
        "1"
    },
    {
        SDL_SCANCODE_2,
        "2"
    },
    {
        SDL_SCANCODE_3,
        "3"
    },
    {
        SDL_SCANCODE_4,
        "4"
    },
    {
        SDL_SCANCODE_5,
        "5"
    },
    {
        SDL_SCANCODE_6,
        "6"
    },
    {
        SDL_SCANCODE_7,
        "7"
    },
    {
        SDL_SCANCODE_8,
        "8"
    },
    {
        SDL_SCANCODE_9,
        "9"
    },
    {
        SDL_SCANCODE_SEMICOLON,
        "SEMICOLON"
    },
    {
        SDL_SCANCODE_EQUALS,
        "EQUALS"
    },
    {
        SDL_SCANCODE_LEFTBRACKET,
        "LEFT"
    },
    {
        SDL_SCANCODE_BACKSLASH,
        "BACKSLASH"
    },
    {
        SDL_SCANCODE_RIGHTBRACKET,
        "RIGHT BRACKET"
    },
    {
        SDL_SCANCODE_A,
        "A"
    },
    {
        SDL_SCANCODE_B,
        "B"
    },
    {
        SDL_SCANCODE_C,
        "C"
    },
    {
        SDL_SCANCODE_D,
        "D"
    },
    {
        SDL_SCANCODE_E,
        "E"
    },
    {
        SDL_SCANCODE_F,
        "F"
    },
    {
        SDL_SCANCODE_G,
        "G"
    },
    {
        SDL_SCANCODE_H,
        "H"
    },
    {
        SDL_SCANCODE_I,
        "I"
    },
    {
        SDL_SCANCODE_J,
        "J"
    },
    {
        SDL_SCANCODE_K,
        "K"
    },
    {
        SDL_SCANCODE_L,
        "L"
    },
    {
        SDL_SCANCODE_M,
        "M"
    },
    {
        SDL_SCANCODE_N,
        "N"
    },
    {
        SDL_SCANCODE_O,
        "O"
    },
    {
        SDL_SCANCODE_P,
        "P"
    },
    {
        SDL_SCANCODE_Q,
        "Q"
    },
    {
        SDL_SCANCODE_R,
        "R"
    },
    {
        SDL_SCANCODE_S,
        "S"
    },
    {
        SDL_SCANCODE_T,
        "T"
    },
    {
        SDL_SCANCODE_U,
        "U"
    },
    {
        SDL_SCANCODE_V,
        "V"
    },
    {
        SDL_SCANCODE_W,
        "W"
    },
    {
        SDL_SCANCODE_X,
        "X"
    },
    {
        SDL_SCANCODE_Y,
        "Y"
    },
    {
        SDL_SCANCODE_Z,
        "Z"
    },
    {
        SDL_SCANCODE_DELETE,
        "DELETE"
    },
    {
        SDL_SCANCODE_CAPSLOCK,
        "CAPS LOCK"
    },
    {
        SDL_SCANCODE_F1,
        "F1"
    },
    {
        SDL_SCANCODE_F2,
        "F2"
    },
    {
        SDL_SCANCODE_F3,
        "F3"
    },
    {
        SDL_SCANCODE_F4,
        "F4"
    },
    {
        SDL_SCANCODE_F5,
        "F5"
    },
    {
        SDL_SCANCODE_F6,
        "F6"
    },
    {
        SDL_SCANCODE_F7,
        "F7"
    },
    {
        SDL_SCANCODE_F8,
        "F8"
    },
    {
        SDL_SCANCODE_F9,
        "F9"
    },
    {
        SDL_SCANCODE_F10,
        "F10"
    },
    {
        SDL_SCANCODE_F11,
        "F11"
    },
    {
        SDL_SCANCODE_F12,
        "F12"
    },
    {
        SDL_SCANCODE_PRINTSCREEN ,
        "PRINT SCREEN"
    },
    {
        SDL_SCANCODE_SCROLLLOCK  ,
        "SCROLL LOCK"
    },
    {
        SDL_SCANCODE_PAUSE,
        "PAUSE"
    },
    {
        SDL_SCANCODE_INSERT,
        "INSERT"
    },
    {
        SDL_SCANCODE_HOME,
        "HOME"
    },
    {
        SDL_SCANCODE_PAGEUP,
        "PAGE UP"
    },
    {
        SDL_SCANCODE_END,
        "END"
    },
    {
        SDL_SCANCODE_PAGEDOWN,
        "PAGE DOWN"
    },
    {
        SDL_SCANCODE_RIGHT,
        "RIGHT"
    },
    {
        SDL_SCANCODE_LEFT,
        "LEFT"
    },
    {
        SDL_SCANCODE_DOWN,
        "DOWN"
    },
    {
        SDL_SCANCODE_UP,
        "UP"
    },
    {
        SDL_SCANCODE_KP_DIVIDE,
        "KEYPAD DIVIDE"
    },
    {
        SDL_SCANCODE_KP_MULTIPLY,
        "KEYPAD MULTIPLY"
    },
    {
        SDL_SCANCODE_KP_MINUS,
        "KEYPAD MINUS"
    },
    {
        SDL_SCANCODE_KP_PLUS,
        "KEYPAD PLUS"
    },
    {
        SDL_SCANCODE_KP_ENTER,
        "KEYPAD ENTER"
    },
    {
        SDL_SCANCODE_KP_1,
        "KEYPAD 1"
    },
    {
        SDL_SCANCODE_KP_2,
        "KEYPAD 2"
    },
    {
        SDL_SCANCODE_KP_3,
        "KEYPAD 3"
    },
    {
        SDL_SCANCODE_KP_4,
        "KEYPAD 4"
    },
    {
        SDL_SCANCODE_KP_5,
        "KEYPAD 5"
    },
    {
        SDL_SCANCODE_KP_6,
        "KEYPAD 6"
    },
    {
        SDL_SCANCODE_KP_7,
        "KEYPAD 7"
    },
    {
        SDL_SCANCODE_KP_8,
        "KEYPAD 8"
    },
    {
        SDL_SCANCODE_KP_9,
        "KEYPAD 9"
    },
    {
        SDL_SCANCODE_KP_0,
        "KEYPAD 0"
    },
    {
        SDL_SCANCODE_KP_PERIOD,
        "KEYPAD PERIOD"
    },
    {
        SDL_SCANCODE_LCTRL,
        "LEFT CONTROL"
    },
    {
        SDL_SCANCODE_LSHIFT,
        "LEFT SHIFT"
    },
    {
        SDL_SCANCODE_LALT,
        "LEFT ALT"
    },
    {
        SDL_SCANCODE_RCTRL,
        "RIGHT CONTROL"
    },
    {
        SDL_SCANCODE_RSHIFT,
        "RIGHT SHIFT"
    },
    {
        SDL_SCANCODE_RALT,
        "RIGHT ALT"
    }
};

GXBind_t    *create_bind               ( void )
{

    // Initialized data
    GXBind_t* ret = calloc(1, sizeof(GXBind_t));

    // Check memory
    {
        #ifndef NDEBUG
            if(ret==(void*)0)
                goto no_memory;
        #endif
    }
    
    return ret;

    // Error handling
    {
        no_memory:
        #ifndef NDEBUG
            g_print_error("[G10] [Bind] Unable to allocate memory in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXInput_t   *create_input              ( void )
{

    // Initialized data
    GXInput_t *ret = calloc(1, sizeof(GXInput_t));

    // Check memory
    {
        #ifndef NDEBUG
            if(ret==(void*)0)
                goto no_memory;
        #endif
    }
    
    return ret;

    // Error handling
    {
        no_memory:
        #ifndef NDEBUG
            g_print_error("[G10] [Bind] Unable to allocate memory in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}
 
GXInput_t   *load_input                ( const char    path[] )
{

    // Argument check
    {
        #ifndef NDEBUG
            if ( path == (void*)0 )
                goto noPath;
        #endif
    }

    // Uninitialized data
    u8*          data;
    size_t       i;

    // Initialized data
    GXInput_t    *ret          = 0;

    // Load up the file
    {
        i    = g_load_file(path, 0, false);
        data = calloc(i + 1, sizeof(u8));
        g_load_file(path, data, false);
    }

    // Construct the input
    ret = load_input_as_json_n(data, i);

    free(data);

    // Error checking
    {
        #ifndef NDEBUG
            if(ret == (void*)0)
                goto no_ret;
        #endif
    }

    // Return the input
    return ret;

    // Error handling
    {

        // Debug only branches
        #ifndef NDEBUG

            // Argument errors
            {
                noPath:
                    g_print_log("[G10] [Input] No path provided to function \"%s\"\n", __FUNCSIG__);
                return 0;
            }

            // G10 errors
            {
                no_ret:
                    g_print_error("[G10] [Input] Failed to parse file \"%s\" in call to function \"%s\"\n", path, __FUNCSIG__);
                return 0;
            }
        #endif
    }
}

GXInput_t   *load_input_as_json        ( char         *token ) 
{

    // Argument check
    {
        #ifndef NDEBUG
            if (token == (void*)0)
                goto no_token;
        #endif
    }

    // Initialized data
    size_t     len = strlen(token);
    GXInput_t* ret = load_input_as_json_n(token, len);

    // Error checking
    {
        if ( ret == (void*)0 )
            goto no_ret;
    }

    return ret;

    // Error handling
    {

        // Debug only branches
        {
            #ifndef NDEBUG

                // Argument errors
                {
                    no_token:
                            g_print_error("[G10] [Input] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
                        return 0;
                }

                // G10 errors
                {
                    no_ret:
                            g_print_error("[G10] [Input] Failed to parse \"token\" in call to function \"%s\"\n", __FUNCSIG__);
                        return 0;
                }
            #endif
        }
    }
}

GXInput_t   *load_input_as_json_n      ( char         *token   , size_t len )
{
    
    // Argument check
    {
        #ifndef NDEBUG
            if (token == (void*)0)
                goto no_token;
        #endif
    }

    // Initialized data
    GXInput_t   *ret         = create_input();
    size_t       token_len   = len,
                 token_count = parse_json(token, token_len, 0, 0),
                 name_len    = 0;
    JSONToken_t *tokens      = calloc(token_count, sizeof(JSONToken_t));

    // JSON results
    char        *name        = 0;
    char       **bind_tokens = 0;

    // Parse JSON
    parse_json(token, token_len, token_count, tokens);

    // Extract data from tokens
    for (size_t i = 0; i < token_count; i++)
    {

        // Parse name
        if (strcmp(tokens[i].key, "name") == 0)
            name = tokens[i].value.n_where;

        // Parse binds array
        else if (strcmp(tokens[i].key, "binds") == 0)
            bind_tokens = tokens[i].value.a_where;


    }

    // Error checking
    {
        #ifndef NDEBUG
            if (name == (void*)0)
                goto no_name;
        #endif
    }
    
    // Construct Input
    {

        // Set name
        {
            name_len = strlen(name);

            ret->name = calloc(len + 1, sizeof(char));

            // Error checking
            {
                #ifndef NDEBUG
                    if(ret->name == (void*)0)
                        goto no_name_mem;
                #endif
            }

            strncpy(ret->name, name, len);
            free(tokens);
        }

        // Set binds
        {

            // Iterate through JSON array
            for (size_t j = 0; bind_tokens[j]; j++)

                // Parse and append each bind from the array
                append_bind(ret, load_bind_as_json(bind_tokens[j]));
        }
    }

    // Warnings
    {
        #ifndef NDEBUG
            if (bind_tokens == (void*)0)
                g_print_error("[G10] [Input] No binds in input set \"%s\"\n", ret->name);
        #endif
    }

    return ret;

    // Error handling
    {

        // Debug only branches
        {
            #ifndef NDEBUG
            
                // Argument errors
                {
                    no_token:
                        g_print_error("[G10] [Input] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
                        return (void *)0;
                }

                // Parsing errors
                {
                    no_name:
                        g_print_error("[G10] [Input] No \"name\" token provided in call to function \"%s\"\n", __FUNCSIG__);
                        return (void *)0;
                }

                // Standard library errors
                {
                    no_name_mem:
                        g_print_error("[Standard library] Failed to allocate memory for \"name\" in call to function \"%s\"", __FUNCSIG__);
                        return (void *)0;
                }

            #endif
        }


    }

}

GXBind_t    *load_bind_as_json         ( char         *token )
{

    // Argument check
    {
        if (token == (void*)0)
            goto no_token;
    }

    GXBind_t* ret = create_bind();
    size_t    len = strlen(token),
              token_count = parse_json(token, len, 0, 0);
    JSONToken_t * tokens = calloc(token_count, sizeof(JSONToken_t));
    parse_json(token, len, token_count, tokens);

    for (size_t i = 0; i < token_count; i++)
    {

        // Parse name
        if (strcmp(tokens[i].key, "name")==0)
        {
            char *name = tokens[i].value.n_where;
            size_t len = strlen(name);
            ret->name = calloc(len + 1, sizeof(char));
            strncpy(ret->name, name, len);
        }
        
        // Parse keys
        else if (strcmp(tokens[i].key, "keys")==0)
        {
            size_t key_count = 0;
            for (key_count; tokens[i].value.a_where[key_count]; ++key_count);

            ret->key_count = key_count;

            ret->keys = calloc(key_count + 1, sizeof(char*));

            for (size_t j = 0; tokens[i].value.a_where[j]; j++)
            {
                char *key_name = tokens[i].value.a_where[j];
                size_t k_len = strlen(key_name);
                
                ret->keys[j] = calloc(k_len + 1, sizeof(char));
                strncpy(ret->keys[j], key_name, k_len);

            }
        }

    }

    free(tokens);

    return ret;

    // Error handling
    {

        // Argument handling
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Bind] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int          register_bind_callback    ( GXBind_t     *bind    , void                *function_pointer )
{
    // Argument check
    {
        if (bind == (void*)0)
            goto noBind;
        if (function_pointer == (void*)0)
            goto noFunPtr;
    }

    if (bind->callback_max == 0)
    {
        bind->callback_max   = 2,
        bind->callbacks      = calloc(bind->callback_max, sizeof(void*));
    }

    if (bind->callback_count + 1 > bind->callback_max)
    {
        bind->callback_max *= 2;
        void **callbacks    = calloc(bind->callback_max, sizeof(void*)),
              *t            = bind->callbacks;

        memcpy(callbacks, bind->callbacks, bind->callback_count * sizeof(void*));
        bind->callbacks = callbacks;

        free(t);
    }

    bind->callbacks[bind->callback_count++] = function_pointer;

    return 0;

    // Error handling
    {
        noBind:
        #ifndef NDEBUG
            g_print_error("[G10] [Input] Null pointer provided for \"bind\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        noFunPtr:
        #ifndef NDEBUG
            g_print_warning("[G10] [Input] Null pointer provided for \"function_pointer\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int          unregister_bind_callback  ( GXBind_t     *bind    , void                *function_pointer )
{
    // TODO: Argument check
    // TODO
    return 0;
    // TODO: Error handling
}

SDL_Scancode find_key                  ( const char   *name )
{
    // Argument check
    {
        if (name == 0)
            goto noName;
    }

    for (size_t i = 0; i < 98; i++)
        if (strcmp(name, keys[i].name) == 0)
            return keys[i].code;
    
    //g_print_warning("[G10] [Input] Unidentified key name \"%s\" passed to funciton \"%s\"\n", name, __FUNCSIG__);
    return (SDL_Scancode)0;

    // Error handling
    {
        noName:
        g_print_error("[G10] [Input] No name provided to function \"%s\"\n", __FUNCSIG__);
        return (SDL_Scancode)0;
    }
}

int          print_all_keys            ( void )
{
    for (size_t i = 0; i < 98; i++)
        printf("\"%s\"\n", keys[i].name);

    return 0;
}

int          print_all_binds           ( GXInput_t    *inputs )
{

    // Argument check
    {
        #ifndef NDEBUG
            if (inputs == (void*)0)
                goto no_inputs;
        #endif
    }

    GXBind_t *i = 0;
    
    while (i->next)
    {
        printf("bind name: %s\n", i->name);

        printf("keys: \n");
        for (size_t j = 0; j < i->key_count; j++)
            printf("\t%s\n", i->keys[j]);

        printf("\ncallback pointers: \n");
        for (size_t j = 0; j < i->callback_count; j++)
            printf("\t%p\n",i->callbacks[j]);

    }

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_inputs:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"inputs\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif 
            return 0;
        }
    }
}

int          process_input             ( GXInstance_t *instance )
{

    // Argument check
    {
        #ifndef NDEBUG
            if (instance == (void*)0)
                goto noInstance;
            if (instance->input == (void*)0)
                goto noInputs;
        #endif  
    }

    // TODO: Reimplement for other libraries?

    // Poll for events 
    while (SDL_PollEvent(&instance->event)) {
        switch (instance->event.type)
        {
            // Mouse events
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            //case SDL_MOUSEWHEEL:
            {
                // Don't fire binds if the mouse isn't lockced
                if (!SDL_GetRelativeMouseMode())
                    break;

                GXBind_t* iter   = instance->input->binds;
                u8        button = 0;

                callback_parameter_t input;
                int                  x_rel = instance->event.motion.xrel,
                                     y_rel = instance->event.motion.yrel;

                input.input_state               = MOUSE;
                input.inputs.mouse_state.xrel   = x_rel;
                input.inputs.mouse_state.yrel   = y_rel;
                input.inputs.mouse_state.button = button;

                while (iter)
                {
                    for (size_t j = 0; j < iter->key_count; j++)
                    {
                        if (strcmp(iter->keys[j], "MOUSE UP")    == 0 && y_rel < 0)
                            fire_bind(iter, input, instance);
                        if (strcmp(iter->keys[j], "MOUSE DOWN")  == 0 && y_rel > 0)
                            fire_bind(iter, input, instance);
                        if (strcmp(iter->keys[j], "MOUSE LEFT") == 0 && x_rel < 0)
                            fire_bind(iter, input, instance);
                        if (strcmp(iter->keys[j], "MOUSE RIGHT") == 0 && x_rel > 0)
                            fire_bind(iter, input, instance);

                    }
                    iter = iter->next;
                }
                break;
            }
            // Keyboard events

            // Key up
            // Key down
            case SDL_KEYDOWN:            

            // Window resize
            case SDL_WINDOWEVENT:
                switch (instance->event.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    g_window_resize(instance);
                    break;
                }
                default:
                    break;
                }
                break;

            // Quit
            case SDL_QUIT:
            {
                instance->running = false;
                break;
            }
        }

    }

    GXBind_t* iter = instance->input->binds;
    callback_parameter_t input;
    while (iter)
    {
        u8* keyboard_state = SDL_GetKeyboardState(NULL);

        for (size_t j = 0; j < iter->key_count; j++)
        {
            SDL_Scancode k = find_key(iter->keys[j]);
            if (k > 512)
                continue;

            if (keyboard_state[k])
            {
                input = (callback_parameter_t) {KEYBOARD, {1}};
                iter->active = true;
                fire_bind(iter, input, instance);
            }
            else if(iter->active == true) {
                input = (callback_parameter_t){ KEYBOARD, {0} };
                iter->active = false;
                fire_bind(iter, input, instance);
            }

        }
        iter = iter->next;
    }

    return 0;

    // Error handling
    {

        // Argument errors
        {
            noInstance:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"instance\" in call to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            noInputs:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] No input in \"instance\" in call to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

    }
}
 
int          fire_bind                 ( GXBind_t     *bind    , callback_parameter_t input, GXInstance_t* instance )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(bind==(void*)0)
                goto no_bind;
        #endif
    }

    for (size_t i = 0; i < bind->callback_count; i++)
    {
        void (*function)(input, instance) = bind->callbacks[i];
        function(input, instance);
    }

    return 0;
    
    // Error handling
    {
        no_bind:
        #ifndef NDEBUG
            g_print_error("[G10] [Bind] Null pointer provided for \"bind\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int          append_bind               ( GXInput_t    *input   , GXBind_t            *bind )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(input == (void*)0)
                goto no_input;
            if (bind == (void*)0)
                goto no_bind;
        #endif
    }

    GXBind_t *i = input->binds;

    if (i == 0)
    {
        input->binds = bind;
        return 0;
    }

    while (i->next)
        i = i->next;

    i->next = bind;

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_input:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"input\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            no_bind:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"input\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXBind_t    *find_bind                 ( GXInput_t    *input   , char                *name )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(input == (void*)0)
                goto no_input;
            if (name == (void*)0)
                goto no_name;
        #endif
    }

    GXBind_t *i = input->binds;

    while (i)
    {
        if (strcmp(i->name, name) == 0)
            return i;
        i = i->next;
    }

    return i;

    // Error handling
    {

        // Argument errors
        {
            no_input:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"input\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            no_name:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"name\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int          remove_bind               ( GXInput_t    *input   , GXBind_t            *bind )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(input == (void*)0)
                goto no_input;
            if (bind == (void*)0)
                goto no_bind;
        #endif
    }

    // TODO
    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_input:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"input\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
            no_bind:
            #ifndef NDEBUG
                g_print_error("[G10] [Input] Null pointer provided for \"bind\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}
 
int          destroy_bind              ( GXBind_t     *bind )
{
    // Argument check
    {
        if (bind == (void*)0)
            goto noBind;
    }

    // TODO: Free everything

    free(bind->name);
    
    for (size_t i = 0; i < bind->key_count; i++)
        free(bind->keys[i]);
    
    if(bind->callbacks)
        free(bind->callbacks);

    free(bind->keys);

    bind->next = 0;

    free(bind);

    return 0;

    // Error handling
    {
        noBind:
        #ifndef NDEBUG
            g_print_error("[G10] [Input] Null pointer provided for \"bind\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int          destroy_input             ( GXInput_t    *input )
{

    // Argument check
    {
        #ifndef NDEBUG
        if (input == (void*)0)
            goto noInput;
        #endif
    }

    // Free the input name
    free(input->name);

    GXBind_t *bind = input->binds;

    while (bind)
    {
        GXBind_t *i = bind;
        bind = bind->next;
        destroy_bind(i);

    }

    free(input);

    return 0;

    // Error handling
    {
        noInput:
        #ifndef NDEBUG
            g_print_error("[G10] [Input] Null poiner provided for \"input\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}
