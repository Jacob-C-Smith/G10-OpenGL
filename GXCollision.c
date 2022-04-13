#include <G10/GXCollision.h>


GXCollision_t* create_collision(void)
{
    // Allocate memory
	GXCollision_t *ret = calloc(1, sizeof(GXCollision_t));

    // Check memory
    {
        #ifndef NDEBUG
            if (ret == (void*)0)
                goto no_mem;
        #endif
    }

	return ret;

    // Error handling
    {
        no_mem:
        #ifndef NDEBUG
            g_print_error("[G10] [Collision] Unable to allocate memory in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}


// Constructors
GXCollision_t* create_collision_from_entities(GXEntity_t* a, GXEntity_t* b)
{

    // Argument check
    {
        if (a == (void*)0)
            goto no_a;
        if (b == (void*)0)
            goto no_b;
    }

	GXInstance_t  *instance = g_get_active_instance();
	GXCollision_t *ret      = create_collision();

	ret->a                  = a,
	ret->b                  = b;

	ret->has_aabb_collision = true;
    ret->begin_tick         = instance->ticks;

    // AABB start callbacks
    {
        for (size_t i = 0; i < a->collider->aabb_start_callback_count; i++)
        {
            void (*function)(collision) = a->collider->aabb_start_callbacks[i];
            function(ret);
        }

        for (size_t i = 0; i < b->collider->aabb_start_callback_count; i++)
        {
            void (*function)(collision) = b->collider->aabb_start_callbacks[i];
            function(ret);
        }
    }

    return ret;
    
    // Error handling
    {

        // Argument errors
        {
            no_a:
            #ifndef NDEBUG
                g_print_error("[G10] [Collision] Null pointer provided for \"a\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;

            no_b:
            #ifndef NDEBUG
                g_print_error("[G10] [Collision] Null pointer provided for \"b\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

bool test_collision ( GXCollision_t *collision )
{
    // Argument check
    {
        #ifndef NDEBUG
            if ( collision == (void *)0 )
                goto no_collision;
        #endif
    }

    // Test AABB
    if (test_aabb(collision))
    {
        collision->has_aabb_collision = true;

        // Test OOB
        if (test_obb(collision))
        {
            collision->has_obb_collision = true;

            // Test convex hull collision
            //if((collision))
            {

            }

        }
    }

    collision->has_collision = collision->has_aabb_collision;

    return collision->has_collision;


    no_collision:
    return false;

    // Error handling
    {

    }
}

bool test_aabb(GXCollision_t* collision)
{
    // Argument check
    {
        #ifndef NDEBUG
	        if (collision == (void*)0)
		        goto no_collision;
    	    if (collision->a == (void*)collision->a)
	    	    goto no_a;
        	if (collision->b == (void*)collision->b)
	        	goto no_b;
        #endif
    }

    GXEntity_t *a   = collision->a,
               *b   = collision->b;

    bool        ret = false;



    return ret;

    no_collision:
    no_a:
    no_b:
    return false;

}

int update_collision ( GXCollision_t *collision )
{

    // Argument check
    {
        if(collision == (void *)0) 
            goto no_collision;
    }

    // Initialized data
	GXEntity_t *a        = collision->a,
		       *b        = collision->b;
    
    // Test collision, do callbacks
    if(test_collision(collision))
    {

        // A callbacks
        for (size_t i = 0; i < a->collider->aabb_callback_count; i++)
        {
            void (*function)(collision) = a->collider->aabb_callbacks[i];
            function(collision);
        }

        // B callbacks
        for (size_t i = 0; i < b->collider->aabb_callback_count; i++)
        {
            void (*function)(collision) = b->collider->aabb_callbacks[i];
            function(collision);
        }
    }

    return 0;

    // Error handling
    {

        // Argument errors
        {
        no_collision:
        no_a:
        no_b:
            #ifndef NDEBUG
                g_print_error("[G10] [Collision] Null pointer provided for \"collision\" in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;

        }
    }
}

int resolve_collision(GXCollision_t* collision)
{


    return 0;
}

// Destructors
int            destroy_collision(GXCollision_t* collision)
{
    // Argument check
    {
        if (collision == (void*)0)
            goto no_collision;
    }

    free(collision);

    return 0;
    // Error handling
    {
        
        // Argument errors
        {
            no_collision:
            #ifndef NDEBUG
                g_print_error("[G10] [Collision] Null pointer provided for \"collision\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}