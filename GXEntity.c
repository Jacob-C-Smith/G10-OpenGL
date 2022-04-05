#include <G10/GXEntity.h>

GXEntity_t *create_entity          ( )
{

    // Allocate space
    GXEntity_t* ret = calloc(1,sizeof(GXEntity_t)); 
    
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
        // Debug only branches
        {
            #ifndef NDEBUG
                no_mem:
                    g_print_error("[G10] [Entity] Out of memory.\n");
                    return 0;
            #endif
        }
    }
}

GXEntity_t *load_entity            ( const char  path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path == (void *)0)
                goto no_path;
        #endif
    }

    // Uninitialized data
    u8         *data;
    GXEntity_t *ret;

    // Initialized data
    size_t      i   = 0;

    #ifndef NDEBUG
        g_print_log("[G10] [Entity] Loading \"%s\".\n", (char*)path);
    #endif	

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret  = load_entity_as_json(data);

    // Finish up
    free(data);

    return ret;

    // Error handling
    {

        // Debug only branches
        {
            #ifndef NDEBUG

                // Argument errors
                {
                    no_path:
                        g_print_error("[G10] [Entity] Null pointer provided for \"path\" in call to function \"%s\"\n", __FUNCSIG__);
                    return 0;
                }

                // G10 Errors
                {
            
                    no_ret:
                        g_print_error("[G10] [G10] [Entity] Failed to parse file \"%s\" in call to function \"%s\"\n", path, __FUNCSIG__);
                    return 0;
                } 
            #endif
        }

    }
}

GXEntity_t *load_entity_as_json      ( char       *token )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(token == (void *)0)
                goto no_token;
        #endif  
    }

    // Initialized data
    GXEntity_t  *ret         = create_entity();
    size_t       len         = strlen(token),
                 token_count = parse_json(token, len, 0, (void*)0),
                 j           = 0;
    JSONToken_t *tokens      = calloc(token_count, sizeof(JSONToken_t));

    // JSON results
    char        *name_token            = 0,
               **parts_array_token     = 0,
               **materials_array_token = 0,
                *transform_token       = 0,
                *shader_token          = 0,
                *rigid_body_token      = 0,
                *collider_token        = 0,
                *rig_token             = 0,
                *particle_system_token = 0;

    // Parse the entity object
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for ( j = 0; j < token_count; j++ )
    {

        // Handle comments
        if      ( strcmp("comment"   , tokens[j].key) == 0 )
        {

            // Check type
            if ( tokens[j].type == JSONstring )
            {

                // Print out comment if in debug mode
                {
                    #ifndef NDEBUG
                        g_print_log("[G10] [Entity] Comment: \"%s\"\n", (char*)tokens[j].value.n_where);
                    #endif
                }

                continue;
            }

            // Type error
            else
                #ifndef NDEBUG
                    goto comment_type_error;
                #endif
            ;

        }

        // Set name
        else if ( strcmp("name"      , tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == JSONstring)
            {
                name_token = tokens[j].value.n_where;  

                continue;
            }

            // Type error check
            #ifndef NDEBUG
            else
                    goto name_type_error;    
            #endif
        }

        // Load and process a mesh
        else if ( strcmp("parts"     , tokens[j].key) == 0 )
        {
            // Check type
            if (tokens[j].type == JSONarray)
            {
                parts_array_token = tokens[j].value.a_where;

                continue;
            }

            // Type error check
            #ifndef NDEBUG
                else
                    goto parts_type_error;
            #endif
        }

        // Process a shader
        else if ( strcmp("shader"    , tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == JSONstring || tokens[j].type == JSONobject)
            {
                shader_token = tokens[j].value.n_where;

                continue;
            }

            // Type error check
            #ifndef NDEBUG
            else
                goto shader_type_error;
            #endif
        }

        // Process materials
        else if ( strcmp("materials" , tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == JSONarray)
            {
                materials_array_token = tokens[j].value.a_where;

                continue;
            }
            
            // Type error check
            #ifndef NDEBUG
            else
                goto materials_type_error;
            #endif
        }

        // Process a transform
        else if ( strcmp("transform" , tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == JSONstring || tokens[j].type == JSONobject)
            {
                transform_token = tokens[j].value.n_where;

                continue;
            }

            // Type error check
            #ifndef NDEBUG
            else
                goto transform_type_error;
            #endif
        }

        // Process a rigidbody
        else if ( strcmp("rigid body", tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == JSONstring || tokens[j].type == JSONobject)
            {
                rigid_body_token = tokens[j].value.n_where;

                continue;
            }

            // Type error check
            #ifndef NDEBUG
                else
                    goto rigid_body_type_error;
            #endif
        }

        // Process a collider
        else if ( strcmp("collider"  , tokens[j].key) == 0 )
        {

            // Check type
            if (tokens[j].type == tokens[j].type == JSONstring || tokens[j].type == JSONobject)
            {
                collider_token = tokens[j].value.n_where;
                continue;
            }

            // Type error check
            #ifndef NDEBUG
                else
                    goto collider_type_error;
            #endif
        }
        
        // Process a rig
        else if ( strcmp("rig"       , tokens[j].key) == 0 )
        {
            rig_token = tokens[j].value.n_where;

            continue;
        }

        loop:continue;
    }

    // Construct the entity from the JSON parser results
    {

        // Set the name
        {

            // Inialized data
            size_t name_len = strlen(name_token);

            // Allocate memory for the string
            ret->name = calloc(name_len + 1, sizeof(u8));

            // Error checking
            {
                #ifndef NDEBUG
                    if(ret->name == (void *)0)
                        goto no_mem;
                #endif
            }

            // Copy name string
            strncpy(ret->name, name_token, name_len);
                
        }

        // Construct and set the parts
        {
            if (parts_array_token)
            {
                // Parse the first part
                ret->parts = (*(char*)parts_array_token[0] == '{') ? load_part_as_json(parts_array_token[0]) : load_part(parts_array_token[0]);

                // Parse subsequent parts
                for (size_t k = 1; parts_array_token[k]; k++)
                    append_part(ret->parts, (*(char*)parts_array_token[k] == '{') ? load_part_as_json(parts_array_token[k]) : load_part(parts_array_token[k]));
            }
        }

        // Construct and set the shader
        {
            if(shader_token)
                ret->shader = (*(char*)shader_token == '{') ? load_shader_as_json(shader_token) : load_shader(shader_token);
        }

        // Construct and set the materials
        {
            if (materials_array_token)
            {

                // Parse the first material
                ret->materials = (*(char*)materials_array_token[0] == '{') ? load_material_as_json(materials_array_token[0]) : load_material(materials_array_token[0]);

                // Parse subsequent materials
                for (size_t k = 1; materials_array_token[k]; k++)
                    append_material(ret->materials, (materials_array_token[k] == '{') ? load_material_as_json(materials_array_token[k]) : load_material(materials_array_token[k]));
            }
        }

        // Construct and set the transform
        {
            if (transform_token)
                ret->transform = (*(char*)transform_token == '{') ? load_transform_as_json(transform_token) : load_transform(transform_token);
        }

        // Construct and set the rigid body
        {
            if (rigid_body_token)
                ret->rigidbody = (*(char*)rigid_body_token == '{') ? load_rigidbody_as_json(rigid_body_token) : load_rigidbody(rigid_body_token);
        }

        // Construct and set the collider
        {
            if (collider_token)
                ret->collider = (*(char*)collider_token == '{') ? load_collider_as_json(collider_token) : load_collider(collider_token);
    
            if (ret->collider)
                if (ret->collider->bv)
                {
                    ret->collider->bv->entity = ret;
                    ret->collider->bv->location = &ret->transform->location;
                }
        
        }

        // Construct and set the rig
        {
            // TODO: 
            if(rig_token)
                ret->rig = (*(char*)rig_token == '{') ? load_rig_as_json((const char*)rig_token) : load_rig(rig_token);
        }

        // Construct and set the particle system
        {
            if(particle_system_token)
                ret->particle_system = (*(char*)particle_system_token == '{') ? load_particle_system_as_json((const char*)particle_system_token) : load_particle_system(rig_token);
        }
    }

    // The tokens are no longer needed
    free(tokens);

    return ret;

    // Error handling
    {

        // Debug only branches
        {
            #ifndef NDEBUG

                // Non fatal JSON type errors 
                {

                    comment_type_error:
                        g_print_warning("[G10] [Entity] \"comment\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Comment not constructed\n", token_types[tokens[j].type], token_types[JSONstring], __FUNCSIG__);
                        goto loop;
                    parts_type_error:
                        g_print_warning("[G10] [Entity] \"parts\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Parts not constructed\n", token_types[tokens[j].type], token_types[JSONarray], __FUNCSIG__);
                        goto loop;
                    shader_type_error:
                        g_print_warning("[G10] [Entity] \"shader\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Shader not constructed\n", token_types[tokens[j].type], token_types[JSONstring], __FUNCSIG__);
                        goto loop;
                    materials_type_error:
                        g_print_warning("[G10] [Entity] \"materials\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Materials not constructed\n", token_types[tokens[j].type], token_types[JSONarray], __FUNCSIG__);
                        goto loop;
                    transform_type_error:
                        g_print_warning("[G10] [Entity] \"transform\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Transform not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                        goto loop;
                    rigid_body_type_error:
                        g_print_warning("[G10] [Entity] \"rigid body\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Rigid body not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                        goto loop;
                    collider_type_error:
                        g_print_warning("[G10] [Entity] \"collider\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Collider not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                        goto loop;
                    rig_type_error:
                        g_print_warning("[G10] [Entity] \"rig\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Rig not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                        goto loop;
                    particle_system_type_error:
                        g_print_warning("[G10] [Entity] \"rigid body\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\". Particle system not constructed\n", token_types[tokens[j].type], token_types[JSONobject], __FUNCSIG__);
                        goto loop;
                }

                // Fatal JSON type errors
                {
                    name_type_error:
                        g_print_error("[G10] [Entity] \"name\" token is of type \"%s\", but needs to be of type \"%s\", in call to function \"%s\"\n", token_types[tokens[j].type], token_types[JSONstring], __FUNCSIG__);
                        return 0;
                }

                // Argument errors
                {
                    no_token:
                        g_print_error("[G10] [Entity] Null pointer provided for \"token\" in call to function \"%s\"\n",__FUNCSIG__);
                        return 0;
                }

            #endif
        }
    }
}

int         integrate_displacement ( GXEntity_t *entity, float  delta_time)
{
    // Argument check
    {
        #ifndef NDEBUG
            if(entity==(void*)0)
                goto noEntity;
            if(entity->transform == (void*)0)
                goto noTransform;
            if (entity->rigidbody == (void*)0)
                goto noRigidbody;
            if (delta_time == 0)
                return 0;
        #endif
    }

    GXRigidbody_t* rigidbody  = entity->rigidbody;
    GXTransform_t* transform  = entity->transform;

    rigidbody->acceleration.x = (float) (rigidbody->forces->x / rigidbody->mass),
    rigidbody->acceleration.y = (float) (rigidbody->forces->y / rigidbody->mass),
    rigidbody->acceleration.z = (float) (rigidbody->forces->z / rigidbody->mass);

    rigidbody->velocity.x     += (float) (rigidbody->acceleration.x ) * delta_time,
    rigidbody->velocity.y     += (float) (rigidbody->acceleration.y ) * delta_time,
    rigidbody->velocity.z     += (float) (rigidbody->acceleration.z ) * delta_time;

    transform->location.x     += (float) ( rigidbody->velocity.x ) * delta_time,
    transform->location.y     += (float) ( rigidbody->velocity.y ) * delta_time,
    transform->location.z     += (float) ( rigidbody->velocity.z ) * delta_time;

    return 0;

    // Argument check
    {
    noEntity:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No entity provided to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    noTransform:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No transform in entity \"%s\" in call to function \"%s\"\n", entity->name, __FUNCSIG__);
        #endif
        return 0;
    noRigidbody:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No rigidbody in entity \"%s\" in call to function \"%s\"\n", entity->name, __FUNCSIG__);
        #endif
        return 0;

    }
}

int         integrateRotation     ( GXEntity_t *entity, float  delta_time )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(entity==(void*)0)
                goto noEntity;
            if(entity->transform == (void*)0)
                goto noTransform;
            if (entity->rigidbody == (void*)0);
                goto noRigidbody;
        #endif
    }

    GXRigidbody_t* rigidbody = entity->rigidbody;
    GXTransform_t* transform = entity->transform;
    
    transform->rotation.u += (float)0.5 * (rigidbody->angular_velocity.u * fabsf(rigidbody->angular_velocity.u));
    transform->rotation.i += (float)0.5 * (rigidbody->angular_velocity.i * fabsf(rigidbody->angular_velocity.i));
    transform->rotation.j += (float)0.5 * (rigidbody->angular_velocity.j * fabsf(rigidbody->angular_velocity.j));
    transform->rotation.k += (float)0.5 * (rigidbody->angular_velocity.k * fabsf(rigidbody->angular_velocity.k));

    // Argument check
    {
    noEntity:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No entity provided to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    noTransform:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No transform in entity \"%s\" in call to function \"%s\"\n", entity->name, __FUNCSIG__);
        #endif
        return 0;
    noRigidbody:
        #ifndef NDEBUG
            g_print_error("[G10] [Physics] No rigidbody in entity \"%s\" in call to function \"%s\"\n", entity->name, __FUNCSIG__);
        #endif
        return 0;

    }
    return 0;
}

int         draw_entity            ( GXEntity_t* entity )
{
    // Argument check
    {
        if (entity == (void*)0)
            goto no_entity;
    }

    // Initilized data
    GXPart_t *i = entity->parts;

    // Draw each part
    while (i)
    {
        // recompute the model matrix
        make_model_matrix(&entity->transform->model_matrix, entity->transform);
        
        // Set the model matrix
        set_shader_transform(entity->shader, entity->transform);

        // If there is a material, set the material
        if (i->material)
            set_shader_material(entity->shader, get_material(entity->materials, i->material));

        // Otherwise, use the default material
        else
            set_shader_material(entity->shader, g_find_material(g_get_active_instance(), "missing material"));

        // If there is a rig, set the rig
        if (entity->rig)
            set_shader_rig(entity->shader, entity->rig);
        
        // Draw the part
        draw_part(i);
        
        i = i->next;
    }

    return 0;

    // Error handling
    {
        no_entity:
        #ifndef NDEBUG
            g_print_error("[G10] [Entity] Null pointer provided for \"entity\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int         destroy_entity(GXEntity_t* entity)
{
    // Argument check
    {
        if (entity == (void*)0)
            goto no_entity;
    }

    // Check to see if items are set before destroying them
    if (entity->name)
        free(entity->name);

    // Destroy parts
    if (entity->parts != (void*)0)
    {
        GXPart_t* part = entity->parts;
        while (part)
        {
            GXPart_t* i = part->next;

            destroy_part(part);

            part = i;
        }
    }

    // Destroy shader
    if (entity->shader != (void*)0)
    {
        GXShader_t* shader = entity->shader;

        destroy_shader(entity->shader);
    }
        

    // Destroy materials
    if (entity->materials != (void*)0)
    {
        GXMaterial_t* material = entity->materials;
        while (material)
        {
            GXMaterial_t* i = material->next;

            destroy_material(material);

            material = i;
        }
    }

    // Destroy transform
    if (entity->transform != (void*)0)
        destroy_transform(entity->transform);

    // Destroy rigidbody
    if (entity->rigidbody != (void*)0)
        destroy_rigidbody(entity->rigidbody);

    // Destroy collider
    if (entity->collider != (void*)0)
        destroy_collider(entity->collider);

    // Destroy rig
    if (entity->rig != (void*)0)
        destroy_rig(entity->rig);

    // Free the entity
    free(entity);

    // Exit
    return 0;

    // Error handling
    {

        // Argument error
        {
            no_entity:
            #ifndef NDEBUG
                g_print_error("[G10] [Entity] Null pointer provided for \"entity\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}
