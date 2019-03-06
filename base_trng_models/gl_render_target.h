#ifndef GL_ENGINE_RENDER_TARGET
#define GL_ENGINE_RENDER_TARGET
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <vector>
#include <memory>
#include "glresourses.h"
#include <iostream>

class glRenderTarget
{

public:
	GLuint depthMap;
	GLuint NormalMap;
	GLuint AlbedoMap;
	GLuint StencilBuffer;

	void InitBuffer(unsigned int WIDTH, unsigned int HEIGHT, float buffer_scale);

	virtual ~glRenderTarget();

	void set();

protected:

	virtual void GenerateBuffers();
	unsigned int width, height;
	GLuint FBO;
//private:



};

class glRenderTargetDeffered : public glRenderTarget
{

public:
	GLuint PositionMap;

	~glRenderTargetDeffered();

private:
	void GenerateBuffers();

};
#endif
