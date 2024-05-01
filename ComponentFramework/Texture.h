#pragma once

#include <glew.h> 
#include <SDL.h>
#include <SDL_image.h>

class Texture {

public:

	Texture();
	~Texture();

	bool LoadImage(const char* filename);
	inline GLuint getTextureID() const { return textureID; }

private:
	GLuint textureID;
};

