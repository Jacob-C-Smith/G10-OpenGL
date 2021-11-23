#include <G10/GXKeyValue.h>

GXKeyValue_t *createKeyValue ( char         *key,  char       *value)
{
    // Argument check
    {
        #ifndef NDEBUG
        if (key == (void*)0)
            goto noKey;
        if (value == (void*)0)
            goto noValue;
        #endif
    }

    // Initialized data
    GXKeyValue_t *ret      = calloc(1, sizeof(GXKeyValue_t));
    size_t        keyLen   = strlen(key),
                  valueLen = strlen(value);

    // Allocate key and value strings
    ret->key          = calloc(1 + keyLen, sizeof(u8));
    ret->value        = calloc(1 + valueLen, sizeof(u8));

    // Copy the key and the value into the strings
    strncpy(ret->key, key, keyLen);
    strncpy(ret->value, value, valueLen);

    // Set next to zero, of course
    ret->next         = 0;
    
    return ret;

    // Error handling
    {
        noKey:
        #ifndef NDEBUG
            gPrintError("[G10] [Key Value] Null pointer provided for key in funciton \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        noValue:
        #ifndef NDEBUG
            gPrintError("[G10] [Key Value] Null pointer provided for value in funciton \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

const char *findValue ( GXKeyValue_t *list, size_t count, char *key )
{
    // Argument check
    {
        #ifndef NDEBUG
            if (list == (void*)0)
                goto noList;

            if (key == (void*)0)
                goto noKey;
        #endif
    }

    // Initialized data
    GXKeyValue_t *i   = list,
                 *ret = (void*)0;

    for (; i->next;)
    {
        if (strcmp(i->key, key) == 0)
            return i->value;
        else
            i = i->next;
    }

    #ifndef NDEBUG
        gPrintLog("[G10] [Key Value] Unable to find value %lld\n.");
    #endif

    return 0;

    // Error handling
    {
        noList:
        #ifndef NDEBUG
            gPrintError("[G10] [Key Value] No list provided to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
        noCount:
        #ifndef NDEBUG
            gPrintError("[G10] [Key Value] No count provided to function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
        noKey:
        #ifndef NDEBUG
            gPrintError("[G10] [Key Value] No key provided to function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}
