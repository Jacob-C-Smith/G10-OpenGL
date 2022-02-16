#include <G10/GXEntity.h>

GXEntity_t *create_entity          ( )
{
    // Allocate space
    GXEntity_t* ret = calloc(1,sizeof(GXEntity_t)); 
    
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
            g_print_error("[G10] [Entity] Out of memory.\n");
        #endif
        return 0;
    }
}

GXEntity_t *load_entity            ( const char  path[] )
{
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
        invalidFile:
            #ifndef NDEBUG
                g_print_error("[G10] [Entity] Failed to load file %s\n", path);
            #endif
        return 0;
    }
}

GXEntity_t *load_entity_as_json      ( char       *token )
{
    // Initialized data
    GXEntity_t  *ret        = create_entity();
    size_t       len        = strlen(token),
                 tokenCount = parse_json(token, len, 0, (void*)0);
    JSONToken_t *tokens     = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the entity object
    parse_json(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Handle comments
        if (strcmp("comment", tokens[j].key) == 0)
        {
            #ifndef NDEBUG
                // Print out comment
                g_print_log("[G10] [Entity] Comment: \"%s\"\n", (char*)tokens[j].value.n_where);
            #endif
            continue;
        }

        // Set name
        else if (strcmp("name", tokens[j].key) == 0)
        {
            // Inialized data
            size_t len = strlen(tokens[j].value.n_where);

            ret->name = calloc(len + 1,sizeof(char));

            // Copy name
            strncpy(ret->name, tokens[j].value.n_where, len);
            ret->name[len] = 0;
            continue;
        }

        // Load and process a mesh
        else if (strcmp("parts", tokens[j].key) == 0)
        {
            ret->parts = (*(char*)tokens[j].value.a_where[0] == '{') ? load_part_as_json((const char*)tokens[j].value.a_where[0]) : load_part(tokens[j].value.a_where[0]);
            
            for (size_t k = 1; tokens[j].value.a_where[k]; k++)
                append_part(ret->parts,(*(char*)tokens[j].value.a_where[k] == '{') ? load_part_as_json((const char*)tokens[j].value.a_where[k]) : load_part(tokens[j].value.a_where[k]));
          
            continue;
        }

        // Process a shader
        else if (strcmp("shader", tokens[j].key) == 0)
        {
            ret->shader = (*(char*)tokens[j].value.n_where == '{') ? load_shader_as_json((const char*)tokens[j].value.n_where) : load_shader(tokens[j].value.n_where);

            continue;
        }

        // Process materials
        else if (strcmp("materials", tokens[j].key) == 0)
        {
            ret->materials = (*(char*)tokens[j].value.a_where[0] == '{') ? load_material_as_json((const char*)tokens[j].value.a_where[0]) : load_material(tokens[j].value.a_where[0]);

            for (size_t k = 1; tokens[j].value.a_where[k]; k++)
                append_material(ret->materials, (*(char*)tokens[j].value.a_where[k] == '{') ? load_material_as_json((const char*)tokens[j].value.a_where[k]) : load_material(tokens[j].value.a_where[k]));

        }

        // Process a transform
        else if (strcmp("transform", tokens[j].key) == 0)
        {
            ret->transform = (*(char*)tokens[j].value.n_where == '{') ? load_transform_as_json(tokens[j].value.n_where) : load_transform(tokens[j].value.n_where);

            continue;
        }

        // Process a rigidbody
        else if (strcmp("rigid body", tokens[j].key) == 0)
        {
            ret->rigidbody = (*(char*)tokens[j].value.n_where == '{') ? load_rigidbody_as_json((char*)tokens[j].value.n_where) : load_rigidbody((const char*)tokens[j].value.n_where);

            continue;
        }

        // Process a collider
        else if (strcmp("collider", tokens[j].key) == 0)
        {
            ret->collider = (*(char*)tokens[j].value.n_where == '{') ? load_collider_as_json((char*)tokens[j].value.n_where) : load_collider((const char*)tokens[j].value.n_where);

            if (ret->collider)
                if (ret->collider->bv)
                {
                    ret->collider->bv->entity = ret;
                    if (&ret->transform->location)
                        ret->collider->bv->location = &ret->transform->location;
                }
            continue;
        }
        else if (strcmp("rig", tokens[j].key) == 0)
        {
            ret->rig = (*(char*)tokens[j].value.a_where[0] == '{') ? load_rig_as_json((const char*)tokens[j].value.a_where[0]) : load_rig(tokens[j].value.a_where[0]);

            
        }
    }




    free(tokens);

    return ret;
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

    rigidbody->acceleration.x =  (rigidbody->forces->x / rigidbody->mass) * delta_time,
    rigidbody->acceleration.y =  (rigidbody->forces->y / rigidbody->mass) * delta_time,
    rigidbody->acceleration.z =  (rigidbody->forces->z / rigidbody->mass) * delta_time;

    rigidbody->velocity.x     += (float) 0.5 * (rigidbody->acceleration.x * fabsf(rigidbody->acceleration.x)),
    rigidbody->velocity.y     += (float) 0.5 * (rigidbody->acceleration.y * fabsf(rigidbody->acceleration.y)),
    rigidbody->velocity.z     += (float) 0.5 * (rigidbody->acceleration.z * fabsf(rigidbody->acceleration.z));

    transform->location.x     += (float) 0.5 * (rigidbody->velocity.x * fabsf(rigidbody->velocity.x)),
    transform->location.y     += (float) 0.5 * (rigidbody->velocity.y * fabsf(rigidbody->velocity.y)),
    transform->location.z     += (float) 0.5 * (rigidbody->velocity.z * fabsf(rigidbody->velocity.z));

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
        if(i->material)
            set_shader_material(entity->shader, get_material(entity->materials, i->material));
        
        // If there is a rig, set the rig
        // TODO: Match the rig to the part
        //if (entity->rigs)
        //    set_shader_rig(entity->shader, entity->rigs);
        
        // Draw the part
        draw_part(i);
        
        i = i->next;
    }

    return 0;
}

int         destroy_entity(GXEntity_t* entity)
{
    // Check to see if items are set before destroying them
    if (entity->name != (void*)0)
        free(entity->name);

    // Destroy parts
    if (entity->parts != (void*)0)
    {
        GXPart_t* part = entity->parts;
        while (part)
        {
            GXPart_t* i = part->next;

            if ( --part->users < 1 )
                destroy_part(part);

            part = i;
        }
    }

    // Destroy shader
    if (entity->shader != (void*)0)
        destroy_shader(entity->shader);

    // Destroy materials
    if (entity->materials != (void*)0)
    {
        GXMaterial_t* material = entity->materials;
        while (material)
        {
            GXMaterial_t* i = material->next;
            
            if (--material->users < 1)
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
}
