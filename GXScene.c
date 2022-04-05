#include <G10/GXScene.h>

GXScene_t  *create_scene     ( void )
{

    // Initialized data
    GXScene_t* ret = calloc(1,sizeof(GXScene_t));

    // Check the memory
    {
        #ifndef NDEBUG
            if (ret == 0)
                goto no_mem;
        #endif
    }
    return ret;

    // Error handling
    {

        // Standard library errors
        {
            no_mem:
            #ifndef NDEBUG
                g_print_error("[Standard Library] Failed to allocate memory in call to function \"%s\".\n", __FUNCSIG__);
            #endif
            return 0;
        }
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
    size_t       l    = 0;
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

    // Argument check
    {
        if (token == (void*)0)
            goto no_token;
    }

    // Uninitialized data
    int          token_count;
    JSONToken_t *tokens;

    // Initialized data
    GXScene_t   *ret           = create_scene();
    size_t       l             = strlen(token),
                 j             = 0;

    char        *name          = 0,
               **entities      = 0,
               **cameras       = 0,
               **lights        = 0,
                *skybox        = 0;

    // Time the function if in debug mode
    {
        #ifndef NDEBUG
            clock_t c = clock();
        #endif
    }

    // Preparse the scene
    token_count      = parse_json(token, l, 0, (void*)0);
    tokens           = calloc(token_count, sizeof(JSONToken_t));

    // Error check
    {
        #ifndef NDEBUG
            if (tokens == (void*) 0)
                goto no_mem;
        #endif
    }

    // Parse the scene
    parse_json(token, l, token_count, tokens);

    // Search through values and pull out relevent information
    for (j = 0; j < token_count; j++)
    {

        // Copy out the name of the scene
        if (strcmp("name", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONstring)
                name = tokens[j].value.n_where;
            else
                goto name_type_error;


            continue;
        }

        // Create entities
        if (strcmp("entities", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONarray)
                entities = tokens[j].value.a_where;
            else
                goto entities_type_error;

            continue;
        }

        // Create cameras
        else if (strcmp("cameras", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONarray)
                cameras = tokens[j].value.a_where;
            else
                goto cameras_type_error;

            continue;
        }

        // Set up lights
        else if (strcmp("lights", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONarray)
                lights = tokens[j].value.a_where;
            else
                goto lights_type_error;

            continue;
        }

        else if (strcmp("skybox", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONstring || tokens[j].type == JSONobject)
                skybox = tokens[j].value.n_where;
            else
                goto skybox_type_error;

            continue;
        }
    }

    // Construct the scene 
    {

        // Set the name
        {
            if (name)
            {
                // Inialized data
                size_t len = strlen(name);

                // Allocate memory for the string
                ret->name = calloc(len + 1, sizeof(u8));

                // Copy name string
                strncpy(ret->name, name, len);
            }
        }

        // Construct and set the entities
        {
            if(entities)
                for (size_t k = 0; entities[k]; k++)
                    append_entity(ret, (*(char*)entities[k] == '{') ? load_entity_as_json(entities[k]) : load_entity(entities[k]));
        }

        // Construct and set the cameras
        {
            if(cameras)
                for (size_t k = 0; cameras[k]; k++)
                    append_camera(ret, (*(char*)cameras[k] == '{')  ? load_camera_as_json(cameras[k])  : load_camera(cameras[k]));
        }

        // Construct and set the lights
        {
            if(lights)
                for (size_t k = 0; cameras[k]; k++)
                    append_light(ret, (*(char*)cameras[k] == '{')   ? load_light_as_json(cameras[k])   : load_light(cameras[k]));
        }

        // Construct and set the skybox
        {
            ret->skybox = (*(char*)skybox == '{') ? load_skybox_as_json(skybox) : load_skybox(skybox);
        }
    }

    // Construct a BVH tree from the scene
    ret->bvh = create_bvh_from_scene(ret);

    // Finish up
    free(tokens);

    // Debug mode log
    {
        #ifndef NDEBUG

            // Write the bounding volume heierarchy to standard out
            {
                print_bv(stdout, ret->bvh, 0);
            }

            // Compute and print how long it took to load the scene 
            {
                #ifndef NDEBUG
                    c = clock() - c;
                    g_print_log("[G10] Loaded scene \"%s\" in %.0f ms\n", ret->name, 1000 * (float)c / CLOCKS_PER_SEC);
                #endif
            }

        #endif
    }

    return ret;

    // Error handling
    {

        // Debug only errors
        // JSON type errors
        {

            name_type_error:
                
            entities_type_error:
                g_print_warning("[G10] [Scene] \"rigid body\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Particle system not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                goto loop;
            cameras_type_error:

            lights_type_error:

            skybox_type_error:

        }

        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"token\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif 
            return 0;
        }
    }
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

int         append_collision ( GXScene_t* scene, GXCollision_t* collision )
{
    
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto null_scene;
            if(collision == 0)
                goto null_collision;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXCollision_t* i = scene->collisions;

    // Check if the head is null. If so, set the head to the collision
    if (i == 0)
    {
        scene->collisions = collision;
        return 0;
    }

    // Search for the end of the linked list
    while ( i )
        i = i->next;

    // Assign next as collision
    i->next = collision;

    return 0;

    // Error handling
    {
        // Two collisions with the same entities cannot exist in the same scene. 
        // This will happen so much, its not worth printing an error message.
        duplicate_collision:
        
        return 0;

        // Argument errors
        {

            // The scene argument was null
            null_scene:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            // The collision argument was null
            null_collision:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"collision\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
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
    GXLight_t  *l           = scene->lights;
    GXShader_t *last_shader = 0;
    
    // Iterate through until en_countering a null entity
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
            if(scene->cameras)
                set_shader_camera(i->shader, scene->cameras);
            
            // Set up image based lighting 
            if(scene->skybox)
                set_shader_ibl(i->shader, scene->skybox);
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
    if (scene->skybox)
        draw_skybox(scene->skybox);


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

int         draw_lights      ( GXScene_t *scene, GXPart_t   *light_part, GXShader_t *shader )
{
    // Argument check
    {
        if ( scene == (void *) 0 )
            goto no_scene;
        if ( light_part == (void *) 0 )
            goto no_light_part;
        if ( shader == (void *) 0 )
            goto no_shader;
    }

    // Initialized data
    GXLight_t      *light  = scene->lights;
    GXTransform_t  *t      = create_transform();

    // Use the right shader
    use_shader(shader);

    // Set camera uniforms
    set_shader_camera(shader, scene->cameras);

    // Iterate through lights
    while (light)
    {

        // Set location, rotation, and scale
        t->location   = light->location;
        t->rotation.i = 1.0;
        t->scale      = (vec3) { 0.2f, 0.2f, 0.2f };

        // Set the transform 
        set_shader_transform(shader, t);

        // Set the color of the light
        set_shader_vec3(shader, "color", light->color);

        // Draw the part
        draw_part(light_part);

        // Iterate
        light = light->next;
    }

    // Get rid of the transform
    destroy_transform(t);

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_scene:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
            no_light_part:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"light_part\" in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
            no_shader:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"shader\" in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int         compute_physics  ( GXScene_t* scene, float       delta_time )
{
    // Commentary
    {
        /*
         * Computing physics is divided into 4 concurrent steps. 
         * 
         * The first step is to apply forces on the object. Forces are summated from array indicies
         * 1-n, where n is forces count. The same is done for torque. New displacement and rotation 
         * derivatives are calculated from net forces, and placed in the 0th element of the array. 
         * 
         * In the second step, the summated forces are integrated to produce new displacement and 
         * angular derivatives. 
         * 
         * The third step is collision detectrion. Collision detection takes place in two steps.
         * Broad phase collision detection happens between aabb's. If a broad phase collision is
         * detected, it is added to the list of collisions. Narrow phase collision detection is
         * performed on actively colliding aabb's using the convex hulls and the Gilbert Johnson
         * Keerthi algorithm. The Expanding Polytope Algorithm is used to compute collision normals.
         * 
         * In the last step, A simple moment impulse solver resolves collisions, and modify forces.
         * 
         * Before the bulk of the calculation, active collisions are checked to ensure they are alive. 
         */
    }

    // Argument checking
    {
        if (scene == (void*)0)
            goto noScene;
        if (delta_time == 0.f)
            goto noDelta;
    }

    // Initialized data
    GXEntity_t    *i = scene->entities;
    GXCollision_t *c = 0;
    // Update active collisions
    {
        // Initialized data
        GXCollision_t* active_collisions = scene->collisions;

        // Iterate through active collisions
        while (active_collisions)
        {
            // Initialized data
            GXEntity_t *a = active_collisions->a,
                       *b = active_collisions->b;

            // Update the collision
            update_collision(active_collisions);
            
            // Is the aabb collision over?
            if (active_collisions->has_aabb_collision==false)
            {
                // Initialized data
                GXInstance_t  *instance = g_get_active_instance();
                

                // Set the ending tick
                active_collisions->end_tick = instance->ticks;

                // End collision callback
                {
                    for (size_t i = 0; i < a->collider->aabb_end_callback_count; i++)
                    {
                        void (*function)(collision) = a->collider->aabb_end_callbacks[i];
                        function(active_collisions);
                    }

                    for (size_t i = 0; i < b->collider->aabb_end_callback_count; i++)
                    {
                        void (*function)(collision) = b->collider->aabb_end_callbacks[i];
                        function(active_collisions);
                    }
                }

                // Remove and destroy the collision
                c = active_collisions;

                goto destroy_co;
            }

            // Is collision over?
            // TODO

            // Iterate
            active_collisions = active_collisions->next;
            continue;

        destroy_co:

            active_collisions = active_collisions->next;
            printf("COLLISION OVER\n");
            destroy_collision(remove_collision(scene, c));
            c = 0;
        }
    }

    // Iterate over entities
    while (i)
    {

        // Check the entity 
        {
            if (i->transform == 0)
                goto noTransform;
            if (i->rigidbody == 0)
                goto noRigidbody;
        }

        // Summate forces
        {
            if (i->rigidbody->active == true)
            {

                i->rigidbody->forces[1].z = -0.098;
                i->rigidbody->forces_count = 1;
            
                summate_forces(i->rigidbody->forces, i->rigidbody->forces_count);
            }
            //summate_forces(i->rigidbody->torques, i->rigidbody->torque_count);
        }

        // Apply forces
        {

            // Integrate displacement and rotation from displacement force and torque
            if (i->rigidbody->active == true)
            {

                // Calculate derivatives of displacement
                integrate_displacement(i, delta_time);

                // Caclulate derivatives of rotation
                //integrateRotation(i, delta_time);            
            }

            noRigidbody:
            if (i->collider == 0)
                goto noCollider;

            // Recompute BV size and BVH tree
            if (i->collider);
                /*if(i->collider->bv)
                {
                    vec3 l = normalize( (vec3) { i->transform->scale.x, 0.f, 0.f } ),
                         f = normalize( (vec3) { 0.f, i->transform->scale.y, 0.f } ),
                         u = normalize( (vec3) { 0.f, 0.f, i->transform->scale.z } );
                        
                    rotate_vec3_by_quaternion(&l, l, i->transform->rotation);
                    rotate_vec3_by_quaternion(&f, f, i->transform->rotation);
                    rotate_vec3_by_quaternion(&u, u, i->transform->rotation);

                  i->collider->bv->dimensions->x = fabs(l.x) + fabs(f.x) + fabs(u.x) + 0.001;
                  i->collider->bv->dimensions->y = fabs(l.y) + fabs(f.y) + fabs(u.y) + 0.001;
                  i->collider->bv->dimensions->z = fabs(l.z) + fabs(f.z) + fabs(u.z) + 0.001;
              }*/
        }

        // Detect collisions
        if (i->collider)
        {
            
            // Initialized data
            GXCollision_t  *collision                = 0;
            GXBV_t         *neighbors                = 0;
            GXEntity_t    **possibe_collisions       = 0;
            size_t          possible_collision_count = 0;

            
            // Broad phase collision detection with bounding volumes
            {

                // Find the neighboring bounding volume for the current entity
                {
                    if (i->collider->bv)
                    {

                        neighbors = find_parent_bv(scene->bvh, i->collider->bv);
                        
                        if (neighbors)
                            neighbors = (neighbors->left == i->collider->bv) ? neighbors->right : neighbors->left;

                        GXBV_t* pbv = i->collider->bv;

                        while (pbv)
                        {
                            resize_bv(pbv);

                            pbv = find_parent_bv(scene->bvh, pbv);
                        }

                        
                    }


                }

                // Extract entities from the bounding volume
                {
                    // Initialized data
                    possible_collision_count = get_entities_from_bv(neighbors, possibe_collisions, 0);

                    // Allocate for possible collisions
                    possibe_collisions = calloc(possible_collision_count, sizeof(void*));

                    // Get collision count
                    if(neighbors)
                        get_entities_from_bv(neighbors, possibe_collisions, possible_collision_count);
                                       
                    // No collisions?
                    if (possible_collision_count == 0)
                        goto no_collisions;

                    // Prune entities that fail the AABB collision test
                    for (size_t j = 0; j < possible_collision_count; j++)
                        if (i->collider->bv != possibe_collisions[j]->collider->bv)
                            if (!checkIntersection(i->collider->bv, possibe_collisions[j]->collider->bv))
                                possibe_collisions[j] = 0;

                    // Collect entities into a contiguous array, to index collisions faster
                    size_t a = 0;
                    for (size_t j = 0; j < possible_collision_count; j++)
                    {
                        if (possibe_collisions[j])
                        {
                            possibe_collisions[a] = possibe_collisions[j];
                            a++;
                        }
                    }

                    possible_collision_count = a;
                    
                }

                // Create a collision object for each collision
                {
                    for (size_t j = 0; j < possible_collision_count; j++)
                    {
                        GXCollision_t *k = scene->collisions;
                        while (k)
                        {
                            if ((k->a == possibe_collisions[j] && k->b == i) ||
                                (k->a == i && k->b == possibe_collisions[j]))
                                goto duplicate_collision;
                        }
                        printf("COLLISION STARTED\n");
                        append_collision(scene, create_collision_from_entities(possibe_collisions[j], i));
                        duplicate_collision:;
                    }
                }

               
            }

            // TODO: Narrow phase collision detection between two colliders using GJK or collision primatives
            {
                GXCollision_t* c = scene->collisions;
                
                while(c)
                {
                    
                    bool result = false;
                    if (c->a->collider->type == convexhull && c->b->collider->type == convexhull)
                        result = GJK(c);

                    if (result == true)
                    {

                    }
                    c = c->next;
                }
            }

            no_collisions:;
            free(possibe_collisions);

        }

        // Resolve constraints
        {
            // TODO

        }

        noTransform:
        noCollider:
        i = i->next;
    }

    return 0;

    // Error handling
    {
        noScene:
        {
            #ifndef NDEBUG
                g_print_warning("[G10] [Scene] Null pointer provided for \"scene\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
        }
        return 0;
        noDelta:
        {
            #ifndef NDEBUG
                g_print_warning("[G10] [Scene] Zero delta time provided for \"delta_time\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
        }
        return 0;
    }
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
    
    // Iterate through list until en_countering a null pointer
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

    // Iterate through list until en_countering a null pointer
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

    // Argument check
    {
        if(scene == (void *)0)
            goto no_scene;
        if (name == (void*)0)
            goto no_name;
    }

    // Create a pointer to the head of the list
    GXLight_t* i = scene->lights;

    // Sanity check
    if (i == 0)
        goto noLights;

    // Iterate through list until en_countering a null pointer
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the light in question, return a pointer
        i = i->next;
    }

    // Unable to locate specified light
    goto noMatch;

    // Error handling
    {

        // Argument errors
        {
            no_scene:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
            no_name:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"name\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

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
            // Initialized data
            GXCamera_t* j  = i->next;
            GXCamera_t* k  = i->next->next;
    
            // Stitch up the linked list 
            i->next        = k;
            j->next        = scene->cameras;
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
        GXLight_t* j  = i->next; // Get a pointer to the next object so as to not lose it after destroying the light
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

        // Argument errors
        {
            nullScene:
            #ifndef NDEBUG
                    g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
    
            nullName:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"name\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
            
        }
    }
}

GXCollision_t *remove_collision ( GXScene_t* scene, GXCollision_t *collision )
{
    // Argument checking 
    {
        if (scene == (void *)0)
            goto no_scene;
        if (collision == (void*)0)
            goto no_collision;
    }


    // Create a pointer to the head of the list
    GXCollision_t* i = scene->collisions;

    // Quick sanity check
    if (i == 0)
        return 0;

    // Check the head
    if (collision == i)
    {
        GXCollision_t* j  = i->next;

        scene->collisions = j;

        return collision;
    }


    while (i->next)
    {
        if (collision == i->next)
        {

            GXCollision_t* j = i->next->next,
                         * k = i->next;

            // Stitch up the linked list 
            i->next = j;
            return k;
        }
        i = i->next;
    }

    return 0;

    // Error handling
    {
        // Argument errors
        {
            no_scene:
                #ifndef NDEBUG
                    g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in call to function \"%s\"\n",__FUNCSIG__);
                #endif
                return 0;
            no_collision:
                #ifndef NDEBUG
                    g_print_error("[G10] [Scene] Null pointer provided for \"collision\" in call to function \"%s\"\n",__FUNCSIG__);
                #endif
                return 0;
        }
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

    // Destroy the cameras
    while (k)
    {
        GXCamera_t* j = k;
        k = k->next;
        destroy_camera(j);
    }

    // Destroy the lights
    while (l)
    {
        GXLight_t* j = l;
        l = l->next;
        destroy_light(j);
    }

    destroy_bv(scene->bvh);

    // Free up the scene
    free(scene);

    return 0;

    // Error handling 
    {

        // Argument errors
        {
            nullScene:
            #ifndef NDEBUG
                g_print_error("[G10] [Scene] Null pointer provided for \"scene\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
         }
    }
}