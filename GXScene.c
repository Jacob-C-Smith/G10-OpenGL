#include <G10/GXScene.h>

GXScene_t  *create_scene     ( )
{
    GXScene_t* ret = calloc(1,sizeof(GXScene_t));

    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Out of memory.\n");
        #endif
        return 0;
    }
}

GXScene_t  *load_scene       ( const char path[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(path == 0)
                goto nullPath;
        #endif
    }

    // Uninitialized data
    int          i;
    GXScene_t   *ret;

    // Initialized data
    size_t       l   = 0;
    char        *data = 0;

    // Load up the file
    i    = g_load_file(path, 0,false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);
    
    // Parse the file as JSON
    ret = load_scene_as_json(data);

    free(data);

    return ret;

    // Error handling
    {
        nullPath:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for \"path\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXScene_t  *load_scene_as_json ( char*      token )
{
    // TODO: Argument check
    // Uninitialized data
    int          tokenCount;
    JSONToken_t *tokens;

    // Initialized data
    GXScene_t   *ret = create_scene();
    size_t       l   = strlen(token);

    // Time the function if in debug mode
    #ifndef NDEBUG
        clock_t c = clock();
    #endif

    // Preparse the scene
    tokenCount       = parse_json(token, l, 0, (void*)0);
    tokens           = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the scene
    parse_json(token, l, tokenCount, tokens);

    // Find and exfiltrate pertinent information.
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Copy out the name of the scene
        if (strcmp("name", tokens[j].key) == 0)
        {
            // Initialized data.
            size_t len = strlen(tokens[j].value.n_where);
            ret->name  = calloc(len+1,sizeof(char));

            // Copy the string
            strncpy(ret->name, tokens[j].value.n_where, len);

            continue;
        }

        // Create entities
        if (strcmp("entities", tokens[j].key) == 0)
        {
            for (size_t k = 0; tokens[j].value.a_where[k]; k++)
                append_entity(ret, (*(char*)tokens[j].value.a_where[k] == '{') ? load_entity_as_json(tokens[j].value.a_where[k]) : load_entity(tokens[j].value.a_where[k]));
            continue;
        }

        // Create cameras
        else if (strcmp("cameras", tokens[j].key) == 0)
        {
            for (size_t k = 0; tokens[j].value.a_where[k]; k++)
                append_camera(ret, (*(char*)tokens[j].value.a_where[k] == '{') ? load_camera_as_json(tokens[j].value.a_where[k]) : load_camera(tokens[j].value.a_where[k]));
            continue;
        }

        // Set up lights
        else if (strcmp("lights", tokens[j].key) == 0)
        {
            for (size_t k = 0; tokens[j].value.a_where[k]; k++)
                append_light(ret, (*(char*)tokens[j].value.a_where[k] == '{') ? load_light_as_json(tokens[j].value.a_where[k]) : load_light(tokens[j].value.a_where[k]));
            continue;
        }

        else if (strcmp("skybox", tokens[j].key) == 0)
        {
            ret->skybox = (*(char*)tokens[j].value.n_where == '{') ? load_skybox_as_json(tokens[j].value.n_where) : load_skybox(tokens[j].value.n_where);
            continue;
        }
    }

    // Construct a BVH tree from the scene
    ret->bvh = create_bvh_from_scene(ret);

    // Finish up
    free(tokens);

    // Compute and print how long it took to load the scene
    #ifndef NDEBUG
    {
        c = clock() - c;
        g_print_log("[G10] Loaded scene \"%s\" in %.0f ms\n", ret->name, 1000 * (float)c / CLOCKS_PER_SEC);
    }
    #endif

    return ret;
}

int         append_entity    ( GXScene_t* scene, GXEntity_t* entity )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(entity == 0)
                goto nullEntity;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXEntity_t* i = scene->entities;

    // Check if the head is null. If so, set the head to the entity
    if (i == 0)
    {
        scene->entities = entity;
        return 0;
    }

    // Search for the end of the linked list, and check every entity 
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, entity->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = entity;

    return 0;

    // Error handling
    {
        // Two entities with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Entity \"%s\" can not be appended to \"%s\" because an entity with that name already exists\n", entity->name, scene->name);
        #endif
        return 0;

        // The scene argument was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The entity argument was null
        nullEntity:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for entity in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int         append_camera    ( GXScene_t* scene, GXCamera_t* camera ) 
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(camera == 0)
                goto nullCamera;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXCamera_t* i = scene->cameras;

    // Check if the head is null. If so, set the head to the entity
    if (i == 0)
    {
        scene->cameras = camera;
        return 0;
    }

    // Search for the end of the linked list
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, camera->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = camera;

    return 0;

    // Error handling
    {
        // Two cameras with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Camera \"%s\" can not be appended to \"%s\" because a camera with that name already exists\n", camera->name, scene->name);
        #endif
        return 0;
    
        // The scene argument was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The camera argument was null
        nullCamera:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for camera in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int         append_light     ( GXScene_t* scene, GXLight_t*  light )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(light == 0)
                goto nullLight;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXLight_t* i = scene->lights;

    // Check if the head is null. If so, set the head to the light
    if (i == 0)
    {
        scene->lights = light;
        return 0;
    }

    // Search for the end of the linked list
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, light->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as light
    i->next = light;

    return 0;

    // Error handling 
    {
        // Two lights with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Light \"%s\" can not be appended to \"%s\" because a light with that name already exists\n", light->name, scene->name);
        #endif
        return 0;

        // The entity argument was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
            return 0;
        // The entity argument was null
        nullLight:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for light in function \"%s\"\n", __FUNCSIG__);
        #endif
            return 0;
    }
}

int         draw_scene       ( GXScene_t* scene ) 
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
        #endif
    }

    // Initialized data
    GXEntity_t *i           = scene->entities;
    GXShader_t* last_shader = 0;
    
    // Iterate through list until we hit nullptr
    while (i)
    {
        // Set up the shader
        {
            // Use it
            if (i->shader != last_shader)
            {
                last_shader = i->shader;
                use_shader(i->shader);
            }

            // Set projection and view matrices, camera position
            set_shader_camera(i->shader, scene->cameras);
            
            // Set up image based lighting 
            // if(scene->skybox)
            //     set_shader_ibl(i->shader, scene->skybox);
        }
        
        // Set up lights
        {
            if(scene->lights)
                set_shader_lights(i->shader, scene->lights, 4);
        }


        // Actually draw the entity
        draw_entity(i);
       
        // Rinse and repeat
        i = i->next;
        
    }
    
    draw_skybox:

    // Lastly, draw the skybox if there is a skybox
    //if (scene->skybox)
        //draw_skybox(scene->skybox, scene->cameras);

    return 0;

    // Error handling
    {
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for scene in function \"%s\".\n", __FUNCSIG__);
        #endif
            return 0;
        noEntities:
        #ifndef NDEBUG
            g_print_warning("[G10] [Scene] No entities in scene \"%s\"\n", scene->name);
        #endif
        goto draw_skybox;
    }
    
}

int         compute_physics  ( GXScene_t* scene, float       deltaTime )
{
    GXEntity_t* i = scene->entities;

    while (i)
    {
        // Check the entity 
        {
            if (i->transform == 0)
                goto noTransform;
            if (i->rigidbody == 0)
                goto noRigidbody;
            if (i->collider == 0)
                ;
        }

        // Summate forces
        summate_forces(i->rigidbody->forces, i->rigidbody->forces_count);
        //summate_forces(i->rigidbody->torque, i->rigidbody->torqueCount);

        // Apply forces
        if (i->rigidbody->active == true)
        {
            // Calculate derivatives of displacement
            integrate_displacement(i, deltaTime);

            // Caclulate derivatives of rotation
            integrateRotation(i, deltaTime);
            
            // Recompute BV size and BVH tree
            vec3 l = normalize( (vec3) { i->transform->scale.x, 0.f, 0.f } ),
                 f = normalize( (vec3) { 0.f, i->transform->scale.y, 0.f } ),
                 u = normalize( (vec3) { 0.f, 0.f, i->transform->scale.z } );

            rotate_vec3_by_quaternion(&l, l, i->transform->rotation);
            rotate_vec3_by_quaternion(&f, f, i->transform->rotation);
            rotate_vec3_by_quaternion(&u, u, i->transform->rotation);

            i->collider->bv->scale->x = fabs(l.x) + fabs(f.x) + fabs(u.x);
            i->collider->bv->scale->y = fabs(l.y) + fabs(f.y) + fabs(u.y);
            i->collider->bv->scale->z = fabs(l.z) + fabs(f.z) + fabs(u.z);
            
        }

        noRigidbody:

        // Detect collisions
        {
            GXBV_t *neighbors;

            // Broad phase collision detection with bounding volumes
            if(i->collider)
                if(i->collider->bv)
                {
                    //neighbors = find_parent_bv(scene->BVH, i->collider->bv);

                }

            // Narrow phase collision detection between two colliders using GJK or collision primatives
            {

            }
            
            
            // If there is indeed a collission, we add the two entities to a list to be resolved
            {

            }
            
        }

        // Resolve constraints
        {
            // 
        }

        noTransform:
        i = i->next;
    }

    return 0;
}

GXEntity_t *get_entity       ( GXScene_t* scene, const char  name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Initialized data
    GXEntity_t* i = scene->entities;

    // Sanity check
    if (i == 0)
        goto noEntities;
    
    // Iterate through list until we hit the entity we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the entity in question, return a pointer
        i = i->next;
    }
    
    // Unable to locate specified entity
    goto noMatch;

    // Error handling
    {
        // There are no entities
        noEntities:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no entities.\n");
        #endif
        return 0;	

        // There is no match
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no entity named \"%s\".", name);
        #endif
        return 0;
    
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided for name in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXCamera_t *get_camera       ( GXScene_t* scene, const char  name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif  
    }

    // Create a pointer to the head of the list
    GXCamera_t* i = scene->cameras;

    // Check to see if there are even any cameras in the first place
    if (i == 0)
        goto noCameras; 

    // Iterate through list until we hit the camera we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the camera in question, return a pointer
        i = i->next;
    }

    // Unable to locate specified camera
    goto noMatch;

    // Error handling
    {
        // There are no cameras
        noCameras:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no cameras in \"%s\".\n", scene->name);
        #endif
        return 0;

        // There is no matching camera
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no camera in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
    
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXLight_t  *get_light        ( GXScene_t* scene, const char  name[] )
{
    // Create a pointer to the head of the list
    GXLight_t* i = scene->lights;

    // Sanity check
    if (i == 0)
        goto noLights;

    // Iterate through list until we hit the light we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the light in question, return a pointer
        i = i->next;
    }

    // Unable to locate specified light
    goto noMatch;

    {
        noLights:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no lights in \"%s\".\n", scene->name);
        #endif
        return 0;

        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no light in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
    }
}

int         set_active_camera ( GXScene_t* scene, const char  name[] )
{
    // Arguments checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }
    // Create a pointer to the head of the list 
    GXCamera_t* i = scene->cameras;
    
    // Quick sanity check
    if (i == 0)
        goto noCameras;

    // Check the head
    if (strcmp(name, i->name) == 0)
        goto noNeed;

    // Find the named camera
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            GXCamera_t* j = i->next;
            GXCamera_t* k = i->next->next;
    
            // Stitch up the linked list 
            i->next = k;
            j->next = scene->cameras;
            scene->cameras = j;
            return 0;
        }
        i = i->next;
    }

    goto noMatch;

    // Error checking
    {
        noCameras:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no cameras, therefore no active camera.\n");
        #endif
        return 0;

        noNeed:
        #ifndef NDEBUG
            g_print_log("[G10] [Scene] \"%s\" is already the active camera.\n", name);
        #endif
        return 0;

        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no camera in \"%s\" named \"%s\".\n", scene->name, name);
        #endif
        return 0;

        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

    }
}

GXEntity_t *remove_entity    ( GXScene_t* scene, const char  name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Create a pointer to the head of the list
    GXEntity_t* i = scene->entities;
    
    // Quick sanity check
    if (i == 0)
        goto noEntities;
    
    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXEntity_t* j = i->next;
        scene->entities = j;
        return i;
    }

    // Destroy the named entity
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0) 
        {
            // Make a copy of the entity that is 2 positions ahead of the current entity
            GXEntity_t* j = i->next->next;
            
            GXEntity_t* ret = i->next;
            
            // Verbose logging
            #ifndef NDEBUG
                g_print_log("[G10] [Scene] Removed entity \"%s\" from \"%s\"\n", name, scene->name);
            #endif
            
            // Stitch up the linked list 
            i->next = j;
            return ret;
        }
        i = i->next;
    }

    goto noMatch;

    // Error handling
    {
        noEntities:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] There are no entities in \"%s\".\n", scene->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] There is no entity in \"%s\" named \"%s\".\n", scene->name, name);
            #endif
            return 0;
        
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXCamera_t *remove_camera    ( GXScene_t* scene, const char  name[] )
{
    // Argument checking
    {
        if (scene == 0)
            goto nullScene;
        if (name == 0)
            goto nullName;
    }

    // Create a pointer to the head of the list
    GXCamera_t* i = scene->cameras;

    // Quick sanity check
    if (i == 0)
        goto noCameras;

    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXCamera_t* j = i->next;
        scene->cameras = j;
        return i;
        return 0;
    }

    // Destroy the named camera
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            // Make a copy of the camera that is 2 positions ahead of the current camera
            GXCamera_t *j   = i->next->next,
                       *ret = i->next; 

            // Verbose logging
            #ifndef NDEBUG
                g_print_log("[G10] [Scene] Destroyed Camera \"%s\"\n", name);
            #endif		

            // Stitch up the linked list 
            i->next = j;

            return ret;
        }
        i = i->next;
    }

    goto noMatch;
    
    // Error handling
    {
        noCameras:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no cameras in \"%s\"", scene->name);
        #endif
        return 0;
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no camera in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
        
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}
    
GXLight_t  *remove_light     ( GXScene_t* scene, const char  name[] )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Create a pointer to the head of the list
    GXLight_t* i = scene->lights;

    // Quick sanity check
    if (i == 0)
        goto noLights;

    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXLight_t* j  = i->next; // Get a pointer to the next object so we don't lose it when we destroy the light
        scene->lights = j;       // Stitch up the linked list

        return 0;
    }

    // Destroy the named light
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            // Make a copy of the light that is 2 positions ahead of the current light
            GXLight_t* j = i->next->next;

            // Delete the light
            destroy_light(i->next);

            // Verbose logging
            #ifndef NDEBUG
                g_print_log("[G10] [Scene] Destroyed light \"%s\"\n", name);
            #endif		

            // Stitch up the linked list 
            i->next = j;
            return 0;
        }
        i = i->next;
    }

    goto noMatch;

    // Error handling
    {
        noLights:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There are no lights in \"%s\"", scene->name);
        #endif
        return 0;
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] There is no light in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;

        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
        }
}

int         destroy_scene    ( GXScene_t* scene )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
        #endif
    }

    // Set sceneID to zero;
    GXEntity_t* i = scene->entities;
    GXCamera_t* k = scene->cameras;
    GXLight_t*  l = scene->lights;

    free(scene->name);

    // Destroy all entities in the scene
    while (i)
    {
        GXEntity_t* j = i;
        i = i->next;
        destroy_entity(j);
    }

    // Zero set the entites pointer
    scene->entities   = (void*)0;

    // Destroy the cameras
    while (k)
    {
        GXCamera_t* j = k;
        k = k->next;
        destroy_camera(j);
    }

    // Zero set the cameras pointer
    scene->cameras = (void*)0;

    // Destroy the lights
    while (l)
    {
        GXLight_t* j = l;
        l = l->next;
        destroy_light(j);
    }

    // Zero set the lights pointer
    scene->lights = (void*)0;

    // Free up the scene
    free(scene);

    return 0;

    // Error handling 
    {
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            g_print_error("[G10] [Scene] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}