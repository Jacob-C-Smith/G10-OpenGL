#include <G10/GXKeyValue.h>

char* findValue(GXKeyValue_t* list, size_t count, size_t key)
{
    for (size_t j = 0; j < count; j++)
    {
        if (list[j].key == key)
            return list[j].value;
    }
    #ifndef NDEBUG
        printf("Unable to find value %lld\n.");
    #endif
    return 0;
}
