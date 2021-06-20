#include <G10/GXRig.h>

GXBone_t* createBone ( )
{
    // Allocate space
	GXBone_t* ret       = malloc(sizeof(GXBone_t));
	
    // Zero set everything
    ret->name           = (void*)0;
	ret->parent         = 0;
	ret->children       = 0;	
	ret->parent         = 0;

	ret->transformation = malloc(sizeof(GXmat4_t));
    
    return ret;
}

GXBone_t* findBone(GXRig_t* rig,  char* name)
{
    // Create a pointer to the head of the list
    GXBone_t* i = rig->bones;

    // Sanity check
    if (i == 0)
        goto noBones;

    return searchBone(rig, name, 0);
    {
        noBones:
            #ifndef NDEBUG
                printf("There are no bones in \"%s\".\n", rig->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                printf("There is no bone in \"%s\" named \"%s\".", rig->name, name);
            #endif
            return 0;
    }
}

GXBone_t* searchBone(GXBone_t* bone, char* name, size_t searchDepth)
{
    // Create a pointer to the head of the list
    GXBone_t* i = bone;

    // Sanity check
    if (i == 0)
        goto noBones;

    // Walk through all the bones 
    while (i)
    {
        // Check the first bone
        if (strcmp(i->name, name) == 0)
            return i;

        // If the bone has any children, search through them too
        if (i->children != 0)
            searchBone(i->children, name, searchDepth - 1);

        // Or continue along
        else 
            i = i->next;


    }
    
    {
        noBones:
            #ifndef NDEBUG
                printf("There are no bones in \"%s\".\n", bone->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                printf("There is no bone in \"%s\" named \"%s\".", bone->name, name);
            #endif
            return 0;
    }
    return 0;
}

int destroyBone(GXBone_t* bone)
{
	return 0;
}
