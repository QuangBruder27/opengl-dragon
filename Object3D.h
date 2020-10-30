#pragma once

//STL includes
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <map>
#include <array>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>

//GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
//NOTE: include before SDL.h

#include <GL/glew.h>

//SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

//OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
//NOTE: must do before including GLM headers
//NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vertex.h"
#include "textureloader.h"

//Model class
class Object3D
{
public:
	//Load a object using assimp
	void loadASSIMP(std::string const& path);

	//Render object 3D
	void render();

	//Set the object buffers
	void setBuffers();
	//Object data 
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Constructor
	Object3D();
	// Variables
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;
	glm::vec4 rotation;
	float speed;
	TextureLoader texture;

private:
	//Vertex buffer object
	GLuint VBO;
	//Vertex array object
	GLuint VAO;
	//Element (index) buffer object
	GLuint EBO;
};