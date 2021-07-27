#include <G10/GXSkybox.h>

const float cubeVerts[] = {
	-1.f,  1.f,  1.f,
	 1.f, -1.f,  1.f,
	 1.f,  1.f,  1.f,
	-1.f, -1.f, -1.f,
	 1.f, -1.f, -1.f,
	-1.f, -1.f,  1.f,
	-1.f,  1.f, -1.f,
	 1.f,  1.f, -1.f
};

const u32 cubeIndicies[] = {
	0, 1, 2,
	1, 3, 4,
	5, 6, 3,
	7, 3, 6,
	2, 4, 7,
	0, 7, 6,
	0, 5, 1,
	1, 5, 3,
	5, 0, 6,
	7, 4, 3,
	2, 1, 4,
	0, 2, 7
};

char skyboxVertexShader[] = "#version 460 core\n" \
"layout (location = 0) in vec3 aPos;\n" \

"out vec3 localPos;\n" \

"uniform mat4 P;\n" \
"uniform mat4 V;\n" \

"void main()\n" \
"{\n" \
"    localPos = aPos;  \n" \
"    gl_Position =  projection * view * vec4(localPos, 1.0);\n" \
"}\n";

char skyboxFragmentShader[] = "#version 460 core\n" \
"out vec4 FragColor;\n" \
"in vec3 localPos;\n" \

"uniform sampler2D equirectangularMap;\n" \

"const vec2 invAtan = vec2(0.1591, 0.3183);\n" \
"vec2 SampleSphericalMap(vec3 v)\n" \
"{\n" \
"	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n" \
"	uv *= invAtan;\n" \
"	uv += 0.5;\n" \
"	return uv;\n" \
"}\n" \

"void main()\n" \
"{\n" \
"vec2 uv = SampleSphericalMap(normalize(localPos));\n" \
"vec3 color = texture(equirectangularMap, uv).rgb;\n" \

"FragColor = vec4(color, 1.0);\n" \
"}\n";


GXSkybox_t* createSkybox(const char path[])
{
	GXSkybox_t *ret  = calloc(1,sizeof(GXSkybox_t));
	ret->texture     = loadPNGImage(path);
	
	glGenFramebuffers(1, &ret->framebuffer);
	glGenRenderbuffers(1, &ret->renderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, ret->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, ret->renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ret->renderbuffer);

	glGenTextures(1, &ret->cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ret->cubemap);

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, (void*)0);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ret->captureViews = calloc(6, sizeof(GXmat4_t));

	glGenVertexArrays(1, &ret->vArray);
	glGenBuffers(1, &ret->vBuffer);
	glGenBuffers(1, &ret->eBuffer);

	glBindVertexArray(ret->vArray);

	glBindBuffer(GL_ARRAY_BUFFER, ret->vBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->eBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndicies), cubeIndicies, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	ret->skyboxShader = malloc(sizeof(GXShader_t));

	compileFromText(ret->skyboxShader, skyboxVertexShader, skyboxFragmentShader);
	
	return ret;
}

int drawSkybox(GXSkybox_t *skybox)
{
	skybox->captureProjection = perspective(M_PI_2, 1.0, 0.1f, 10.f);

	skybox->captureViews[0] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) {  1.0f,  0.0f,  0.0f }, (GXvec3_t) { 0.0f, -1.0f,  0.0f });
	skybox->captureViews[1] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) { -1.0f,  0.0f,  0.0f }, (GXvec3_t) { 0.0f, -1.0f,  0.0f });
	skybox->captureViews[2] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) {  0.0f,  1.0f,  0.0f }, (GXvec3_t) { 0.0f,  0.0f,  1.0f });
	skybox->captureViews[3] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) {  0.0f, -1.0f,  0.0f }, (GXvec3_t) { 0.0f,  0.0f, -1.0f });
	skybox->captureViews[4] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) {  0.0f,  0.0f,  1.0f }, (GXvec3_t) { 0.0f, -1.0f,  0.0f });
	skybox->captureViews[5] = lookAt((GXvec3_t) { 0.0f, 0.0f, 0.0f }, (GXvec3_t) {  0.0f,  0.0f, -1.0f }, (GXvec3_t) { 0.0f, -1.0f,  0.0f });

	useShader(skybox->skyboxShader);
	setShaderInt(skybox->skyboxShader, "equirectangularMap", 0);
	setShaderMat4(skybox->skyboxShader, "P", &skybox->captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_TEXTURE_2D, skybox->texture);

	for (unsigned int i = 0; i < 6; ++i)
	{
		setShaderMat4(skybox->skyboxShader, "V", &skybox->captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox->cubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glBindVertexArray(skybox->vArray);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}
