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
    
    // Is the collision over?
    if (checkIntersection(a->collider->bv, b->collider->bv) == false)
        collision->has_aabb_collision = false;

    // AABB callbacks
    if(collision->has_aabb_collision)
    {
        for (size_t i = 0; i < a->collider->aabb_callback_count; i++)
        {
            void (*function)(collision) = a->collider->aabb_callbacks[i];
            function(collision);
        }

        for (size_t i = 0; i < b->collider->aabb_callback_count; i++)
        {
            void (*function)(collision) = b->collider->aabb_callbacks[i];
            function(collision);
        }
    }

    /* Impulse momentum
	// Resolve collision
    vec3        lastAVel = a->rigidbody->velocity,
                lastBVel = b->rigidbody->velocity,
               *aVel     = &a->rigidbody->velocity,
               *bVel     = &b->rigidbody->velocity;

    float       aMass    = a->rigidbody->mass,
                bMass    = b->rigidbody->mass;

    float       c        = (aMass - bMass) / (aMass + bMass),
                d        = (bMass - aMass) / (aMass + bMass),
                e        = (2 * bMass) / (aMass + bMass),
                f        = (2 * aMass) / (aMass + bMass);
    
    aVel->x              = (lastAVel.x * c) + (lastBVel.x * e);
    aVel->y              = (lastAVel.y * c) + (lastBVel.y * e);
    aVel->z              = (lastAVel.z * c) + (lastBVel.z * e);
    aVel->w              = (lastAVel.w * c) + (lastBVel.w * e);

    bVel->x              = (lastAVel.x * f) + (lastBVel.x * d);
    bVel->y              = (lastAVel.y * f) + (lastBVel.y * d);
    bVel->z              = (lastAVel.z * f) + (lastBVel.z * d);
    bVel->w              = (lastAVel.w * f) + (lastBVel.w * d);
    */

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_collision:
            #ifndef NDEBUG
                g_print_error("[G10] [Collision] Null pointer provided for \"collision\" in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;

        }
    }
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