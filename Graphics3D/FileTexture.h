#ifndef FILE_TEXTURE_H
#define FILE_TEXTURE_H
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

class FileTexture
{
public:
	unsigned int ID;
	FileTexture(const char* path);
	void use(GLenum textureUnit = GL_TEXTURE0);
};
#endif