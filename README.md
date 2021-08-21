# G10 Game Engine Documentation
###### NOTE: This is a hobby project to explore game engine development. As such, this repository may be updated at random intervals. This code may not be the most efficient or safe. 
## G10 structures, definitions, and functions
### ≡ Scene ≡ 
A scene is the highest level object in G10.
```c
// Scene definintion in G10
struct GXScene_s {
    char       *name;
    GXEntity_t *entities;
    GXCamera_t *cameras;
    GXLight_t  *lights;
};
typedef struct GXScene_s GXScene_t;

// Scene functions
GXScene_t  *createScene         ( );                                    
GXScene_t  *loadScene           ( const char path[] );                  

int         appendEntity        ( GXScene_t *scene, GXEntity_t *entity );
int         appendCamera        ( GXScene_t *scene, GXCamera_t *camera );
int         appendLight         ( GXScene_t *scene, GXLight_t  *light  );

int         drawScene           ( GXScene_t *scene );                    

int         computePhysics      ( GXScene_t *scene, float deltaTime );

GXEntity_t *getEntity           ( GXScene_t *scene, const char name[] ); 
GXCamera_t *getCamera           ( GXScene_t *scene, const char name[] ); 
GXLight_t  *getLight            ( GXScene_t *scene, const char name[] ); 

int         setActiveCamera     ( GXScene_t *scene, const char name[] );

GXEntity_t *removeEntity        ( GXScene_t *scene, const char name[] );
GXCamera_t *removeCamera        ( GXScene_t *scene, const char name[] );
GXLight_t  *removeLight         ( GXScene_t *scene, const char name[] );

int         destroyScene        ( GXScene_t *scene );                   
```
#### ⌠createScene⌡
```createScene()``` will create an empty scene.
Returns a pointer to the scene.
#### ⌠loadScene⌡
```loadScene(const char path[])``` will load a scene from a JSON file at ```path```. The function will automatically allocate space for and populate entities, while appending them to the linked list.

Returns a pointer to the loaded scene.
#### ⌠appendEntity⌡
```appendEntity(GXScene_t* scene, GXEntity_t* entity)``` will place the parameter ```entity``` at the end of the linked list of objects pointed at by ```scene```.
#### ⌠drawScene⌡
```drawScene(GXScene_t* scene)``` will draw all objects in the linked list, so long as the ```GXE_rDrawable``` flag is set in the ```flags``` member of the respective entity, and all other criteria for drawing are met.
#### ⌠getEntity⌡
```GXEntity_t* getEntity (GXScene_t* scene, const char name[])``` will locate an entity in a ```scene``` by ```name```.
Returns a pointer to the found entity. On fail, returns ```nullptr```.
#### ⌠removeEntity⌡
```int removeEntity(GXScene_t* scene, const char name[])``` will remove an entity from the ```scene``` by ```name```, deallocating the entity, and finally repair the linked list.
#### ⌠destroyScene⌡
```int destroyScene(GXScene_t* scene)``` will destroy a ```scene```, deallocating all entities in the linked list, setting ```head``` to ```nullptr```, and finally deallocating the ```scene```.
### ≡ Cameras ≡
A camera is an object that contains information on how to render a scene. 
```c
// Camera definition in G10
struct GXCamera_s {

    //Name
    char              *name;

    // View
    GXvec3_t           where;
    GXvec3_t           target;
    GXvec3_t           up;

    // Projection
    float              fov;
    float              near;
    float              far;
    float              aspectRatio;

    // Matricies
    GXmat4_t           view;
    GXmat4_t           projection;

    // Next
    struct GXCamera_s *next;
};
typedef struct GXCamera_s GXCamera_t;

// Camera functions
GXCamera_t*     createCamera            ( );
GXCamera_t*     loadCamera              ( const char *path );
GXCamera_t*     loadCameraAsJSON        ( char       *token );
GXmat4_t        perspective             ( float       fov,    float aspect,     float near,   float far );
extern void     AVXPerspective          ( GXmat4_t   *ret,    float fov,        float aspect, float near, float far );
void            computeProjectionMatrix ( GXCamera_t *camera );
extern void     AVXView                 ( GXvec3_t   *eye,    GXvec3_t *target, GXvec3_t *up, GXmat4_t *result );
inline GXmat4_t lookAt                  ( GXvec3_t    eye,    GXvec3_t target,  GXvec3_t up );
inline void     computeViewMatrix       ( GXCamera_t *camera );
int             destroyCamera           ( GXCamera_t *camera );
```
#### ⌠createCamera⌡
```GXCamera_t* createCamera(GXvec3_t where, GXvec3_t target, GXvec3_t up, float fov, float near, float far, float aspectRatio)``` will create a camera with the specified arguments.
### ≡ Light ≡
A light is used to calculate lighting.
```c
struct GXLight_s
{
    const char       *name;
    GXvec4_t          color;    
    GXvec4_t          location; 
    struct GXLight_s *next;
};
typedef struct GXLight_s GXLight_t;

GXLight_t *createLight     (  );
GXLight_t *loadLight       ( const char  path [] );
GXLight_t *loadLightAsJSON ( char       *token   ); 
int        destroyLight    ( GXLight_t*  light   ); 
```

### ≡ Entities ≡
An entity is any object contained within a scene.
```c
// Entity definition in G10
struct GXEntity_s
{
    size_t         flags;     // Tells G10 how to handle the entity
    char          *name;      // An identifier for the entity
    GXMesh_t      *mesh;      // A mesh
    GXShader_t    *shader;    // A shader
    GXTransform_t *transform; // A transform
    GXRigidbody_t *rigidbody; // A rigidbody
    GXCollider_t  *collider;  // A collider

    struct GXEntity_s* next;  // Points to the next entity.
};
typedef struct GXEntity_s GXEntity_t;

// Entity functions
GXEntity_t* createEntity     ( );
GXEntity_t* loadEntity       ( const char  path[] );
GXEntity_t* loadEntityAsJSON ( char       *token  );
int         drawEntity       ( GXEntity_t *entity );
int         assignTexture    ( GXShader_t *shader, const char uniform[] );
int         destroyEntity    ( GXEntity_t *entity );
```
#### ⌠createEntity⌡
```GXEntity_t* createEntity ( )``` will create an empty entity.
Returns a pointer to the created entity.
#### ⌠drawEntity⌡
```int drawEntity ( GXEntity_t* entity )``` will draw ```entity```, as long as the ```GXE_rDrawable``` flag is set in ```flags```.
#### ⌠loadEntity⌡
```GXEntity_t* loadEntity ( const char path[] )``` will load an entity from ```path```, allocate space for the members, and populate them. The JSON format for entities is detailed further in the documentation.
Returns a pointer to the created entity.
#### ⌠assignTexture⌡
```int assignTexture( GXShader_t* shader, const char uniform[] )``` assigns the texture in entity to a glsl uniform.
#### ⌠destroyEntity⌡
```int destroyEntity( GXEntity_t* entity )``` will destroy an entity and depopulate all of its members.
### ≡ Mesh ≡
A mesh is a container for sets of points used in rendering.
```c

// Stores information about a part of a mesh
struct GXPart_s
{
    char               *name;

    GXMaterial_t       *material;

    size_t              vertexGroups;

    GLuint              vertexArray;

    GLuint              vertexBuffer;
    GLuint              elementBuffer;

    GLuint              elementsInBuffer;

    struct GXPart_s    *next;
};
typedef struct GXPart_s GXPart_t;

// Stores information about a mesh
struct GXMesh_s
{
    char        *name;
    GXPart_t    *parts;
};
typedef struct GXMesh_s GXMesh_t;

GXMesh_t    *createMesh       ( );
GXPart_t    *createPart       ( );

GXMesh_t    *loadMesh         ( const char   path[] );
GXMesh_t    *loadMeshAsJSON   ( char        *token );

GXPart_t    *loadPart         ( const char   path[] );
GXPart_t    *loadPartAsJSON   ( char        *token );

GXPart_t    *getPart          ( GXMesh_t    *mesh, const char  name[] );

int          appendPart       ( GXMesh_t    *mesh, GXPart_t   *part );

int          drawMesh         ( GXMesh_t    *mesh, GXShader_t *shader );
int          drawPart         ( GXPart_t    *part );

int          removePart       ( GXMesh_t    *mesh, const char  name [] );

int          destroyMesh      ( GXMesh_t    *mesh );
int          destroyPart      ( GXPart_t    *part );
```
#### ⌠unloadMesh⌡
```int unloadMesh(GXMesh_t* mesh)``` will depopulate all members of ```mesh```, and deallocate ```mesh```
### ≡ Shader ≡
A shader is a program used by OpenGL to render an object.
```c
// Shader definition in G10
// Contains information about a shader
struct GXShader_s
{
    char         *name;
    unsigned int  shaderProgramID;
    size_t        requestedDataFlags;
    size_t        requestedDataCount;
    GXKeyValue_t *requestedData;
};
typedef struct GXShader_s GXShader_t;

GXShader_t *loadShader        ( const char  path[] );
GXShader_t *loadShaderAsJSON  ( char       *token );
GXShader_t *loadCompileShader ( const char  vertexPath[], const char fragmentPath[], const char shaderName[] );
int         useShader         ( GXShader_t *shader );
void        setShaderInt      ( GXShader_t *shader, const char name[], int       value );
void        setShaderFloat    ( GXShader_t *shader, const char name[], float     value );
void        setShaderVec3     ( GXShader_t *shader, const char name[], GXvec3_t  vector );
void        setShaderMat4     ( GXShader_t *shader, const char name[], GXmat4_t *m );
int         unloadShader      ( GXShader_t *shader );
```

#### ⌠loadShader⌡
```GXShader_t* loadShader(const char vertexPath[], const char fragmentPath[])``` will load a vertex and fragment shader from ```vertexPath``` and ```fragmentPath``` respectively; then compile and link the shader.
Returns a pointer to the loaded ```GXShader_t```.
#### ⌠useShader⌡
```int useShader(GXShader_t* shader)``` will tell OpenGL to use the ```shader``` for draw calls.
#### ⌠setShaderInt⌡
```void setShaderInt(GXShader_t* shader, const char name[], int value)``` will set an int in ```shader``` to ```value```.
#### ⌠setShaderFloat⌡
```void setShaderFloat(GXShader_t* shader, const char name[], float value)``` will set a float in ```shader``` to ```value```.
#### ⌠setShaderVec3⌡
```void setShaderVec3 ( GXShader_t* shader, const char name[], GXvec3_t vector );``` will set a vector in ```shader``` to ```value```.
#### ⌠setShaderMat4⌡
```void setShaderMat4(GXShader_t* shader, const char name[], GXmat4_t* m)``` will set a float in ```shader``` to ```value```.
#### ⌠unloadShader⌡
```int unloadShader(GXShader_t* shader)``` will depopulate all members of ```shader``` and deallocate the shader.
### ≡ Textures ≡
A texture is an image that is mapped onto a mesh.
```c
// Contains information about a texture
struct GXTexture_s
{
    char        *name;

    unsigned int textureID;
    unsigned int textureUnitIndex;

    size_t       width;
    size_t       height;
};
typedef struct GXTexture_s GXTexture_t;

// Keeps track of what textures are being used by the GPU
struct GXTextureUnit_s
{
    GXTexture_t **activeTextureBlock;
    unsigned int  activeTextureCount;
};
typedef struct GXTextureUnit_s GXTextureUnit_t;


// Constructors
GXTexture_t *createTexture            ( );                    // ✅ Creates an empty texture
GXTexture_t *loadTexture              ( const char path[] );  // ✅ Loads a texture from the disk

// Texture streaming
unsigned int loadTextureToTextureUnit ( GXTexture_t *image ); // ✅ Binds a texture to a texture unit, returns texture unit

// Destructors
int          unloadTexture            ( GXTexture_t *image ); // ✅ Unloads a texture and all of its contents


```
#### ⌠createTexture⌡
```GXTexture_t* createTexture ( )``` will create an empty texture.
#### ⌠loadTexture⌡
```GXTexture_t* loadTexture   ( const char path[] )``` will load a texture and deduce the file type.
#### ⌠unloadTexture⌡
```int unloadTexture(GXTexture_t* image)``` will depopulate all members of ```image``` and deallocate ```image```.

### ≡ Transform ≡
A transform is 
```c
// Transform definition in G10
struct GXTransform_s
{
	GXvec3_t location;
	GXvec3_t rotation;
	GXvec3_t scale;

	GXmat4_t modelMatrix;
};
typedef struct GXTransform_s GXTransform_t;

// Transform functions
GXTransform_t* createTransform( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale );
int            unloadTransform( GXTransform_t* transform );
```

#### ⌠createTransform⌡
```GXTransform_t* createTransform(GXvec3_t location, GXvec3_t rotation, GXvec3_t scale)``` will create a GXtransform from the provided information.

#### ⌠unloadTransform⌡
```int unloadTransform(GXTransform_t* transform)``` will destroy the designated GXtransform.
### ≡ Material ≡
A material is 
```c
// Material definition in G10
struct GXMaterial_s {
	GXTexture_t* albedo;
	GXTexture_t* normal;
	GXTexture_t* roughness;
	GXTexture_t* metallic;
	GXTexture_t* AO;
};
typedef struct GXMaterial_s GXMaterial_t;

// Material functions
GXMaterial_t* createMaterial( );
int           assignMaterial( GXMaterial_t* material, GXShader_t* shader );
int           unloadMaterial( GXMaterial_t* material ); 
```

#### ⌠createMaterial⌡
```GXMaterial_t* createMaterial( )``` will create a GXmaterial from the provided information.

#### ⌠unloadTransform⌡
```int assignMaterial( GXMaterial_t* material, GXShader_t* shader )``` will destroy the designated GXtransform.

## File Loaders
At the time of writing, G10 supports the following formats
- BMP
- PNG
- JPG
- PLY

Further image and mesh types may be included in the future, so long as the loader can parse the data into the correct ```struct```. Future candidates for supported file types, at the time of writing are
- FBX
- HEIC

## Scene JSON Format
Scenes may be stored in a JSON file
```json
{
    "comment"   : "Made by Jacob Smith on 1/4/2021 at 22:30 GMT-7:00",
	"name"      : "Example Scene"
    "entities"  : [
            "gameassets/asset1.json",
            "gameassets/asset2.json",
            "gameassets/asset3.json" 
        ],
    "camera"   : {
        "where"       : [ 2,2,2 ],
        "target"      : [ 0,0,0 ],
        "up"          : [ 0,0,-1 ],
        "fov"         : 90,
        "near"        : 0.1,
        "far"         : 100,
        "aspectRatio" : 1.77777777 
    }
	"lights"   : [
		    {
               "name"     : "Light 1",
               "color"    : [ 10.0, 10.0, 10.0 ],
               "position" : [ 3.0, 3.0, 3.0 ]
            },
			{
               "name"     : "Light 2",
               "color"    : [ 10.0, 10.0, 10.0 ],
               "position" : [ 3.0, 3.0, 3.0 ]
            }
	]
}
```
### ⌠comment⌡
The ```"comment"``` label corresponds to a string with arbitrary information about the entity. The comment is ignored by the code unless in ```debugmode```.
### ⌠name⌡ 
The ```"name"``` label corresponds to a stri
### ⌠entities⌡
The ```"entities"``` label corresponds to an array that contains paths to entity json files.
### ⌠camera⌡
The ```"camera"``` label corresponds to an object that contains information about the scenes camera.
#### ⌠where⌡
The ```"where"``` label corresponds to where the camera is in the scene.
#### ⌠target⌡
The ```"target"``` label corresponds to the location the camera is looking at.
#### ⌠up⌡
The ```"up"``` label corresponds to the up direction in the scene.
#### ⌠fov⌡
The ```"fov"``` label corresponds to the fov of the camera in degrees.
### ⌠near⌡
The ```"near"``` label corresponds to the distance of the near clipping plane.
#### ⌠far⌡
The ```"far"``` label corresponds to the distance of the far clipping plane.
#### ⌠aspectRatio⌡
The ```"aspectRatio"``` label corresponds to the aspect ratio of the camera. If unset or set to zero, aspect ratio defaults to 16:9.

## Entity JSON Format
Entities may be stored in a JSON file.
```json
{
	"comment"   : "Made by Jacob Smith on 1/3/2020 at 9:15 GMT-7:00",
	"name"      : "Cube",
	"mesh"      : "gameassets/meshes/cube.ply",
	"shader"    : {
		"vertexPath"   : "gameassets/shaders/pbrv.glsl",
		"fragmentPath" : "gameassets/shaders/pbrf.glsl"
	},
	"transform" : {
		"location"                   : [ 0,0,0 ],
		"rotation"                   : [ 0,0,0 ],
		"scale"                      : [ 1,1,1 ]
	},
	"material" : {
		"albedo"                           : "gameassets/textures/basicCube/albedo.png"    ,
		"normal"                           : "gameassets/textures/basicCube/normal.png",
		"metallic"                         : "gameassets/textures/basicCube/metal.png"     ,
		"roughness"                        : "gameassets/textures/basicCube/rough.png"     ,
		"AO"                               : "gameassets/textures/basicCube/ao.png"       
	}
}
```
### ⌠comment⌡
The ```"comment"``` label corresponds to a string with arbitrary information about the entity. The comment is ignored unless the ```GXDEBUGCOMMENTS``` flag in ```GXDEBUGMODE``` is set.
### ⌠name⌡
The ```"name"``` label corresponds to the value to be assigned to the name of the entity.
### ⌠mesh⌡
The ```"mesh"``` label corresponds to an object with information about the ```mesh```.
### ⌠shader⌡
The ```"shader"``` label corresponds to an object with information about the ```shader```.
#### ⌠vertexShaderRelativePath⌡
The ```"vertexShaderRelativePath"``` label corresponds to the relative path of the vertex shader.
#### ⌠fragmentShaderRelativePath⌡
The ```"fragmentShaderRelativePath"``` label corresponds to the relative path of the fragment shader.
### ⌠transform⌡
The ```"shader"``` label corresponds to an object with information about the ```shader```.
#### ⌠location⌡
The ```"location"``` label corresponds to the location of the entity.
#### ⌠rotation⌡
The ```"rotation"``` label corresponds to the rotation of the entity.
#### ⌠scale⌡
The ```"scale"``` label corresponds to the scale of the entity.
### ⌠material⌡
The ```"material"``` label corresponds to an object with information about the ```material```.
#### ⌠albedo⌡
The ```"albedo"``` label corresponds to the path of an albedo texture.
#### ⌠normal⌡
The ```"normal"``` label corresponds to the path of an normal texture.
#### ⌠metallic⌡
The ```"metallic"``` label corresponds to the path of an metallic texture.
#### ⌠roughness⌡
The ```"roughness"``` label corresponds to the path of an roughness texture.
#### ⌠AO⌡
The ```"AO"``` label corresponds to the path of an AO texture.
## Demo application
Below is the source code of one approach to creating a game with G10. 
```c
#include <stdbool.h>
#include <G10/GXScene.h>

int main(int argc, const char[] argv)
{
	{
		/* Declare your variables, initialize your data, set up your window manager, and load OpenGL here */
	}

	/* Initialized data */
	bool running = true;
	GXScene_t* scene = loadScene("gameassets/level1/scene.json");
	
	/* Game loop */
	while(running)
	{
		{
			/* Process input, clear the screen, check for exit, etc */
		}
		
		drawScene(scene);

		/* Swap windows if using VSync, etc */
	}

	/* Clean up G10 */
	destroyScene(scene);
	
	{
		/* Clean up OpenGL, window manager, allocated memory, etc */
	}
	return 0;
}
```