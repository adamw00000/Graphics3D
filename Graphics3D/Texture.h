#pragma once
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

class Texture
{
public:
	unsigned int ID;
	Texture(const GLchar* path, GLint format);
	void use(GLenum textureUnit);
};

