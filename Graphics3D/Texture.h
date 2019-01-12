#pragma once
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

class Texture
{
public:
	unsigned int ID;
	Texture(const char* path);
	void use(GLenum textureUnit = GL_TEXTURE0);
};

