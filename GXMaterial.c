#include <G10/GXMaterial.h>

GXMaterial_t* createMaterial()
{
    // Initialized data
    GXMaterial_t* ret = malloc(sizeof(GXMaterial_t));

    // Check allocated memory
    if (ret == NULL)
        return ret;

    // Zero set the textures
    ret->albedo    = (void*)0;
    ret->normal    = (void*)0;
    ret->roughness = (void*)0;
	ret->metallic  = (void*)0;
	ret->AO        = (void*)0;

    return ret;
}

int assignMaterial(GXMaterial_t* material, GXshader_t* shader)
{
    // Set the texture uniforms
    setShaderInt(shader, "albedoMap", 0);
    setShaderInt(shader, "normalMap", 1);
    setShaderInt(shader, "metallicMap", 2);
    setShaderInt(shader, "roughnessMap", 3);
    setShaderInt(shader, "aoMap", 4);

    // Bind the texture units to the textureIDs
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->albedo->textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material->normal->textureID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material->metallic->textureID);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, material->roughness->textureID);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, material->AO);

    return 0;
}

int unloadMaterial(GXMaterial_t* material)
{
    // Unload all of the textures
    unloadTexture(material->albedo);
    unloadTexture(material->normal);
    unloadTexture(material->metallic);
    unloadTexture(material->roughness);
    unloadTexture(material->AO);

    // Free the material
    free(material);

    return 0;
}
