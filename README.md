# G10 Game Engine Documentation
###### NOTE: This is a hobby project to explore game engine development. As such, this repository may be updated at random intervals. This code may not be the most efficient or safe. 
## G10 structures, definitions, and functions
### ≡ Scene ≡ 
A scene is the highest level object in G10.
```c
// Scene definintion in G10
struct GXscene_s {
	GXentity_t* head;
	GXcamera_t* camera;
};
typedef struct GXscene_s GXscene_t;
```
There are several functions to manipulate a scene, all of which are listed below.
```c
GXscene_t*  createScene  ();
GXscene_t*  loadScene    (const char path[]);
int         appendEntity (GXscene_t* scene, GXentity_t* entity);
int         drawScene    (GXscene_t* scene);
GXentity_t* getEntity    (GXscene_t* scene, const char name[]);
int         removeEntity (GXscene_t* scene, const char name[]);
int         destroyScene (GXscene_t* scene);
```
#### ⌠createScene⌡
```createScene()``` will create an empty scene.
Returns a pointer to the scene.
#### ⌠loadScene⌡
```loadScene(const char path[])``` will load a scene from a JSON file at ```path```. The function will automatically allocate space for and populate entities, while appending them to the linked list.

Returns a pointer to the loaded scene.
#### ⌠appendEntity⌡
```appendEntity(GXscene_t* scene, GXentity_t* entity)``` will place the parameter ```entity``` at the end of the linked list of objects pointed at by ```scene```.
#### ⌠drawScene⌡
```drawScene(GXscene_t* scene)``` will draw all objects in the linked list, so long as the ```GXE_rDrawable``` flag is set in the ```flags``` member of the respective entity.
#### ⌠getEntity⌡
```GXentity_t* getEntity (GXscene_t* scene, const char name[])``` will locate an entity in a ```scene``` by ```name```.
Returns a pointer to the found entity. On fail, returns ```nullptr```
#### ⌠removeEntity⌡
```int removeEntity(GXscene_t* scene, const char name[])``` will remove an entity from the ```scene``` by ```name```, deallocating the entity, and finally repair the linked list.
#### ⌠destroyScene⌡
```int destroyScene(GXscene_t* scene)``` will destroy a ```scene```, deallocating all entities in the linked list, setting ```head``` to ```nullptr```, and finally deallocating the ```scene```.
### ≡ Cameras ≡
A camera is an object that contains information on how to render a scene. 
```c
// Camera definition in G10
struct GXcamera_s {
	
	// View
	GXvec3_t where;
	GXvec3_t target;
	GXvec3_t up;

	// Projection
	float fov;
	float near;
	float far;
	float aspectRatio;

	// Matricies
	GXmat4_t view;
	GXmat4_t projection;
};
typedef struct GXcamera_s GXcamera_t;
```
There is only one function that should be used for a camera.

```c
GXcamera_t* createCamera(GXvec3_t where, GXvec3_t target, GXvec3_t up, float fov, float near, float far, float aspectRatio)
```
#### ⌠createCamera⌡
```GXcamera_t* createCamera(GXvec3_t where, GXvec3_t target, GXvec3_t up, float fov, float near, float far, float aspectRatio)``` will create a camera with the specified arguments.
### ≡ Entities ≡
An entity is any object contained within a scene.
```c
// Entity definition in G10
struct GXEntity_s
{
	GXsize_t           ID;
	GXsize_t           flags;
	char*              name;
	GXmesh_t*          mesh;
	GXshader_t*        shader;
	GXtexture_t*       UV;
	GXTransform_t*     transform;
	struct GXEntity_s* next;
};
typedef struct GXEntity_s GXentity_t;
```
There are several functions to manipulate an Entity, all of which are listed below.
```c
GXentity_t* createEntity  (GXsize_t flags);
int         drawEntity    (GXentity_t* entity);
GXentity_t* loadEntity    (const char path[]);
int         assignTexture (GXentity_t* entity, const char uniform[]);
int         destroyEntity (GXentity_t* entity);
```
#### ⌠createEntity⌡
```GXentity_t* createEntity (GXsize_t flags)``` will create an entity with ```flags``` set.
Returns a pointer to the created entity.
#### ⌠drawEntity⌡
```int drawEntity (GXentity_t* entity)``` will draw ```entity```, as long as the ```GXE_rDrawable``` flag is set in ```flags```.
#### ⌠loadEntity⌡
```GXentity_t* loadEntity (const char path[])``` will load an entity from ```path```, allocate space for the members, and populate them. The JSON format for entities is detailed further in the documentation.
Returns a pointer to the created entity.
#### ⌠assignTexture⌡
```int assignTexture(GXshader_t* shader, const char uniform[])``` assigns the texture in entity to a glsl uniform.
#### ⌠destroyEntity⌡
```int destroyEntity(GXentity_t* entity)``` will destroy an entity and depopulate all of its members.
### ≡ Mesh ≡
A mesh is a container for sets of points used in rendering.
```c
// /Mesh definition in G10
struct GXMesh_s
{
	GXsize_t ID;
	faces_t faces;
	GXsize_t geometricVerticesCount;
	GXsize_t textureCoordinatesCount;
	GXsize_t vertexNormalsCount;
	GXsize_t facesCount;
	// Array
	GLuint vertexArray;
	// geometric vertecies
	GLuint vertexBuffer;
	GLuint elementBuffer;
	// textures
	GLuint textureBuffer;
};
typedef struct GXMesh_s GXmesh_t;
```
There is only 1 mesh manipulation function
```c
int unloadMesh(GXmesh_t* mesh);
```
#### ⌠unloadMesh⌡
```int unloadMesh(GXmesh_t* mesh)``` will depopulate all members of ```mesh```, and deallocate ```mesh```
### ≡ Shader ≡
A shader is a program used by OpenGL to render an object.
```c
// Shader definition in G10
struct GXShader_s
{
	unsigned int shaderProgramID;
};
typedef struct GXShader_s GXshader_t;
```
There are 6 functions to manipulate shaders
```c
GXshader_t* loadShader     (const char vertexShaderPath[], const char fragmentShaderPath[]);
int         useShader      (GXshader_t* shader);
void        setShaderInt   (GXshader_t* shader, const char name[], int value);
void        setShaderFloat (GXshader_t* shader, const char name[], float value);
void        setShaderMat4  (GXshader_t* shader, const char name[], GXmat4_t* m);
int         unloadShader   (GXshader_t* shader);
```

#### ⌠loadShader⌡
```GXshader_t* loadShader(const char vertexShaderPath[], const char fragmentShaderPath[])``` will load a vertex and fragment shader from ```vertexShaderPath``` and ```fragmentShaderPath``` respectively; then compile and link the shader.
Returns a pointer to the loaded ```GXshader_t```.
#### ⌠useShader⌡
```int useShader(GXshader_t* shader)``` will tell OpenGL to use the ```shader``` for draw calls.
#### ⌠setShaderInt⌡
```void setShaderInt(GXshader_t* shader, const char name[], int value)``` will set an int in ```shader``` to ```value```
#### ⌠setShaderFloat⌡
```void setShaderFloat(GXshader_t* shader, const char name[], float value)``` will set a float in ```shader``` to ```value```
#### ⌠setShaderMat4⌡
```void setShaderMat4(GXshader_t* shader, const char name[], GXmat4_t* m)``` will set a float in ```shader``` to ```value```
#### ⌠unloadShader⌡
```int unloadShader(GXshader_t* shader)``` will depopulate all members of ```shader``` and deallocate the shader.
### ≡ Textures ≡
A texture is an image that is mapped onto a mesh.
```c
// Texture definition in G10
struct GXtexture_s
{
	unsigned int textureID;
	GXsize_t width;
	GXsize_t height;
	u8* pixelData;
};
typedef struct GXtexture_s GXtexture_t;
```
There is 1 function to manipulate a texture
```c
int unloadTexture(GXtexture_t* image);
```
#### ⌠unloadTexture⌡
```int unloadTexture(GXtexture_t* image)``` will depopulate all members of ```image``` and deallocate ```image```
### ≡ Transform ≡
A transform is 
```c
// Transform definition in G10
struct GXtransform_s
{
	GXvec3_t location;
	GXvec3_t rotation;
	GXvec3_t scale;

	GXmat4_t modelMatrix;
};
typedef struct GXtransform_s GXtransform_t;
```
There are 2 functions to manipulate a transform
```c
GXtransform_t* createTransform(GXvec3_t location, GXvec3_t rotation, GXvec3_t scale);
int            unloadTransform(GXtransform_t* transform);
```

#### ⌠createTransform⌡
```GXtransform_t* createTransform(GXvec3_t location, GXvec3_t rotation, GXvec3_t scale)``` will create a GXtransform from the provided information

#### ⌠unloadTransform⌡
```int unloadTransform(GXtransform_t* transform)``` will destroy the designated GXtransform

## File Loaders
At the time of writing, G10 supports the following formats
- Bitmap
- PNG
- JPG
- OBJ

Further image and mesh types may be included in the future, so long as the loader can parse the data into the correct ```struct```. Future candidates for supported file types, at the time of writing are
- PLY
- FBX
## Entity JSON Format
Entities may be stored in a JSON file.
```json
{
	"comment" : "Made by Jacob Smith on 8/17/2020 at 21:45 GMT-7:00",
	"flags"   : 31,
	"name"    : "something",
	"mesh"    : {
		"format"                     : "OBJ",
		"relativePath"               : "gameassets/something/.obj"
	},
	"texture" : {
		"format"                     : "PNG",
		"relativePath"               : "gameassets/something/.png"
	},
	"shader"  : {
		"vertexShaderRelativePath"   : "gameassets/something/.glvs",
		"fragmentShaderRelativePath" : "gameassets/something/.glfs"
	}
}
```
### comment
The ```"comment"``` label corresponds to a string with arbitrary information about the entity. The comment is ignored by the code unless in ```debugmode```
### flags
The ```"flags"``` label corresponds to the value to be assigned to flags of the entity.
### name
The ```"name"``` label corresponds to the value to be assigned to the name of the entity.
### mesh
The ```"mesh"``` label corresponds to an object with information about the ```mesh```
#### ⌠format⌡
The ```"format"``` label corresponds to a string containing the file format
#### ⌠relativePath⌡
The ```"relativePath"``` label corresponds to the relative path of the mesh.
### texture
The ```"texture"``` label corresponds to an object with information about the ```texture```
#### ⌠format⌡
The ```"format"``` label corresponds to a string containing the file format
#### ⌠relativePath⌡
The ```"relativePath"``` label corresponds to the relative path of the texture.
### shader
The ```"shader"``` label corresponds to an object with information about the ```shader```
#### ⌠vertexShaderRelativePath⌡
The ```"vertexShaderRelativePath"``` label corresponds to the relative path of the vertex shader.
#### ⌠fragmentShaderRelativePath⌡
The ```"fragmentShaderRelativePath"``` label corresponds to the relative path of the fragment shader.