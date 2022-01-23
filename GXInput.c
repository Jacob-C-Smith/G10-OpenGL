#include <G10/GXInput.h>

typedef struct { SDL_KeyCode code; const char* name; } kn_t;
const kn_t keys[117] = {
    {
        SDLK_RETURN,
        "RETURN"
    },
    {
        SDLK_ESCAPE,
        "ESCAPE"
    },
    {
        SDLK_BACKSPACE,
        "BACKSPACE"
    },
    {
        SDLK_TAB,
        "TAB"
    },
    {
        SDLK_SPACE,
        "SPACE"
    },
    {
        SDLK_EXCLAIM,
        "EXCLAIMATION MARK"
    },
    {
        SDLK_QUOTEDBL,
        "DOUBLE QUOTE"
    },
    {
        SDLK_HASH,
        "HASH"
    },
    {
        SDLK_PERCENT,
        "PERCENT"
    },
    {
        SDLK_DOLLAR,
        "DOLLAR"
    },
    {
        SDLK_AMPERSAND,
        "AMPERSAND"
    },
    {
        SDLK_QUOTE,
        "SINGLE QUOTE"
    },
    {
        SDLK_LEFTPAREN,
        "LEFT PARENTHESIS"
    },
    {
        SDLK_RIGHTPAREN,
        "RIGHT PARENTHESIS"
    },
    {
        SDLK_ASTERISK,
        "ASTERISK"
    },
    {
        SDLK_PLUS,
        "PLUS"
    },
    {
        SDLK_COMMA,
        "COMMA"
    },
    {
        SDLK_MINUS,
        "MINUS"
    },
    {
        SDLK_PERIOD,
        "PERIOD"
    },
    {
        SDLK_SLASH,
        "SLASH"
    },
    {
        SDLK_0,
        "0"
    },
    {
        SDLK_1,
        "1"
    },
    {
        SDLK_2,
        "2"
    },
    {
        SDLK_3,
        "3"
    },
    {
        SDLK_4,
        "4"
    },
    {
        SDLK_5,
        "5"
    },
    {
        SDLK_6,
        "6"
    },
    {
        SDLK_7,
        "7"
    },
    {
        SDLK_8,
        "8"
    },
    {
        SDLK_9,
        "9"
    },
    {
        SDLK_COLON,
        "COLON"
    },
    {
        SDLK_SEMICOLON,
        "SEMICOLON"
    },
    {
        SDLK_LESS,
        "LESS"
    },
    {
        SDLK_EQUALS,
        "EQUALS"
    },
    {
        SDLK_GREATER,
        "GREATER"
    },
    {
        SDLK_QUESTION,
        "QUESTION MARK"
    },
    {
        SDLK_AT,
        "AT"
    },
    {
        SDLK_LEFTBRACKET,
        "LEFT"
    },
    {
        SDLK_BACKSLASH,
        "BACKSLASH"
    },
    {
        SDLK_RIGHTBRACKET,
        "RIGHT BRACKET"
    },
    {
        SDLK_CARET,
        "CARET"
    },
    {
        SDLK_UNDERSCORE,
        "UNDERSCORE"
    },
    {
        SDLK_BACKQUOTE,
        "BACK QUOTE"
    },
    {
        SDLK_a,
        "A"
    },
    {
        SDLK_b,
        "B"
    },
    {
        SDLK_c,
        "C"
    },
    {
        SDLK_d,
        "D"
    },
    {
        SDLK_e,
        "E"
    },
    {
        SDLK_f,
        "F"
    },
    {
        SDLK_g,
        "G"
    },
    {
        SDLK_h,
        "H"
    },
    {
        SDLK_i,
        "I"
    },
    {
        SDLK_j,
        "J"
    },
    {
        SDLK_k,
        "K"
    },
    {
        SDLK_l,
        "L"
    },
    {
        SDLK_m,
        "M"
    },
    {
        SDLK_n,
        "N"
    },
    {
        SDLK_o,
        "O"
    },
    {
        SDLK_p,
        "P"
    },
    {
        SDLK_q,
        "Q"
    },
    {
        SDLK_r,
        "R"
    },
    {
        SDLK_s,
        "S"
    },
    {
        SDLK_t,
        "T"
    },
    {
        SDLK_u,
        "U"
    },
    {
        SDLK_v,
        "V"
    },
    {
        SDLK_w,
        "W"
    },
    {
        SDLK_x,
        "X"
    },
    {
        SDLK_y,
        "Y"
    },
    {
        SDLK_z,
        "Z"
    },
    {
        SDLK_DELETE,
        "DELETE"
    },
    {
        SDLK_CAPSLOCK,
        "CAPS LOCK"
    },
    {
        SDLK_F1,
        "F1"
    },
    {
        SDLK_F2,
        "F2"
    },
    {
        SDLK_F3,
        "F3"
    },
    {
        SDLK_F4,
        "F4"
    },
    {
        SDLK_F5,
        "F5"
    },
    {
        SDLK_F6,
        "F6"
    },
    {
        SDLK_F7,
        "F7"
    },
    {
        SDLK_F8,
        "F8"
    },
    {
        SDLK_F9,
        "F9"
    },
    {
        SDLK_F10,
        "F10"
    },
    {
        SDLK_F11,
        "F11"
    },
    {
        SDLK_F12,
        "F12"
    },
    {
        SDLK_PRINTSCREEN ,
        "PRINT SCREEN"
    },
    {
        SDLK_SCROLLLOCK  ,
        "SCROLL LOCK"
    },
    {
        SDLK_PAUSE,
        "PAUSE"
    },
    {
        SDLK_INSERT,
        "INSERT"
    },
    {
        SDLK_HOME,
        "HOME"
    },
    {
        SDLK_PAGEUP,
        "PAGE UP"
    },
    {
        SDLK_END,
        "END"
    },
    {
        SDLK_PAGEDOWN,
        "PAGE DOWN"
    },
    {
        SDLK_RIGHT,
        "RIGHT"
    },
    {
        SDLK_LEFT,
        "LEFT"
    },
    {
        SDLK_DOWN,
        "DOWN"
    },
    {
        SDLK_UP,
        "UP"
    },
    {
        SDLK_KP_DIVIDE,
        "KEYPAD DIVIDE"
    },
    {
        SDLK_KP_MULTIPLY,
        "KEYPAD MULTIPLY"
    },
    {
        SDLK_KP_MINUS,
        "KEYPAD MINUS"
    },
    {
        SDLK_KP_PLUS,
        "KEYPAD PLUS"
    },
    {
        SDLK_KP_ENTER,
        "KEYPAD ENTER"
    },
    {
        SDLK_KP_1,
        "KEYPAD 1"
    },
    {
        SDLK_KP_2,
        "KEYPAD 2"
    },
    {
        SDLK_KP_3,
        "KEYPAD 3"
    },
    {
        SDLK_KP_4,
        "KEYPAD 4"
    },
    {
        SDLK_KP_5,
        "KEYPAD 5"
    },
    {
        SDLK_KP_6,
        "KEYPAD 6"
    },
    {
        SDLK_KP_7,
        "KEYPAD 7"
    },
    {
        SDLK_KP_8,
        "KEYPAD 8"
    },
    {
        SDLK_KP_9,
        "KEYPAD 9"
    },
    {
        SDLK_KP_0,
        "KEYPAD 0"
    },
    {
        SDLK_KP_PERIOD,
        "KEYPAD PERIOD"
    },
    {
        SDLK_LCTRL,
        "LEFT CONTROL"
    },
    {
        SDLK_LSHIFT,
        "LEFT SHIFT"
    },
    {
        SDLK_LALT,
        "LEFT ALT"
    },
    {
        SDLK_RCTRL,
        "RIGHT CONTROL"
    },
    {
        SDLK_RSHIFT,
        "RIGHT SHIFT"
    },
    {
        SDLK_RALT,
        "RIGHT ALT"
    }
};

GXBind_t* create_bind(void)
{
    GXBind_t* ret = calloc(1, sizeof(GXBind_t));

    // TODO: Check memory
    
    return ret;
}

GXInput_t* create_input(void)
{
    GXInput_t *ret = calloc(1, sizeof(GXInput_t));

    // TODO: Check memory

    return ret;
    // TODO: Error handling
}

GXInput_t* load_input(const char path[])
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==0)
                goto noPath;
        #endif
    }

    // Uninitialized data
    u8*          data;
    size_t       i;

    // Initialized data
    GXInput_t    *ret          = 0;
    FILE         *f            = fopen(path, "rb");  

    // Load up the file
    i = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_input_as_json(data);

    free(data);

    // Return the input
    return ret;

    // Error handling
    {
        noPath:
        #ifndef NDEBUG
            g_print_log("[G10] [Input] No path provided to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXInput_t* load_input_as_json(char* token)
{
    // TODO: Argument check
    GXInput_t    *ret = create_input();
    size_t token_len = strlen(token),
           token_count = parse_json(token, token_len, 0, 0);
    JSONToken_t *tokens = calloc(token_count, sizeof(JSONToken_t));

    parse_json(token, token_len, token_count, tokens);

    for (size_t i = 0; i < token_count; i++)
    {
        if (strcmp(tokens[i].key, "name") == 0)
        {
            char* name = tokens[i].value.n_where;
            size_t len = strlen(name);

            ret->name = calloc(len+1, sizeof(char));
            
            strncpy(ret->name, name, len);

        }
        else if (strcmp(tokens[i].key, "binds") == 0)
        {
            for (size_t j = 0; tokens[i].value.a_where[j]; j++)
            {
                append_bind(ret, load_bind_as_json(tokens[i].value.a_where[j]));
            }
        }
    }
    return ret;
    // TODO: Error handling
}

GXBind_t* load_bind_as_json(char* token)
{
    // TODO: Argument check
    GXBind_t* ret = create_bind();
    size_t    len = strlen(token),
              token_count = parse_json(token, len, 0, 0);
    JSONToken_t * tokens = calloc(token_count, sizeof(JSONToken_t));
    parse_json(token, len, token_count, tokens);

    for (size_t i = 0; i < token_count; i++)
    {
        if (strcmp(tokens[i].key, "name")==0)
        {
            char *name = tokens[i].value.n_where;
            size_t len = strlen(name);
            ret->name = calloc(len + 1, sizeof(char));
            strncpy(ret->name, name, len);
        }
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

    return ret;
    // TODO: Error handling
}

int register_bind_callback(GXBind_t* bind, void* function_pointer)
{
    // TODO: Argument check
    return 0;
    // TODO: Error handling
}

int unregister_bind_callback(GXBind_t* bind, void* function_pointer)
{
    // TODO: Argument check
    return 0;
    // TODO: Error handling
}


SDL_KeyCode find_key(const char* name)
{
    // TODO: Argument check
    for (size_t i = 0; i < 117; i++)
        if (strcmp(name, keys[i].name) == 0)
            return keys[i].code;
    
    // TODO: Write an error

    return 0;

    // TODO: Error handling
}

int print_all_keys(void)
{
    // TODO: Argument check
    for (size_t i = 0; i < 117; i++)
        printf("\"%s\"\n", keys[i].name);

    return 0;
}

int print_all_binds(GXInput_t* inputs)
{
    // TODO: Argument check
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
    // TODO: Error handling
}

int process_input(GXInstance_t* instance)
{
    // TODO: Argument check

    return 0;
    // TODO: Error handling
}

int fire_bind(GXBind_t* bind)
{
    // TODO: Argument check

    for (size_t i = 0; i < bind->callback_count; i++)
    {
        void (*function)() = bind->callbacks[i];
        function();
    }

    return 0;
    // TODO: Error handling
}

int append_bind(GXInput_t* input, GXBind_t* bind)
{
    // TODO: Argument check
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
    // TODO: Error handling
}

int find_bind(GXInput_t* input, char* name)
{
    // TODO: Argument check
    GXBind_t *i = input->binds;

    while (i->next)
        if (strcmp(i->name, name) == 0)
            return input;

    return 0;
    // TODO: Error handling
}

int remove_bind(GXInput_t* input, GXBind_t* bind)
{
    // TODO: Argument check
    return 0;
    // TODO: Error handling
}

int destroy_bind(GXBind_t* bind)
{
    // TODO: Argument check
    free(bind->name);
    // TODO: Free everything
    bind->key_count = 0;
    bind->callback_count = 0;
    bind->callback_max = 0;
    bind->next = 0;

    return 0;
    // TODO: Error handling
}

int destroy_input(GXInput_t* input)
{
    // TODO: Argument check
    return 0;
    // TODO: Error handling
}
