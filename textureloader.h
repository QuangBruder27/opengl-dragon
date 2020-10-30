
#pragma once
//SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

//Texture class
class TextureLoader
{
public:
	//Load textures from file
	void load(const std::string fileName);

	//Set buffers 
	void setBuffers();

	//Bind the texture
	void bind() const;

	//SDL surface and texture handle
	SDL_Surface* tex;
	GLuint texture;
};