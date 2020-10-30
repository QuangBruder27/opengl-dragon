#pragma once


//Custom includes
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Vertes struct
struct Vertex
{
	//Vertex data
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
};
