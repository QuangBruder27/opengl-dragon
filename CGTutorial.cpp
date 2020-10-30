// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem veränderten API schon in der Version 3
// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"


#include "Obj3D.hpp"

// Ab Uebung5 werden objloader.hpp und cpp benoetigt
#include "objloader.hpp"
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

// Wuerfel und Kugel
#include "texture.hpp"
#include "textureloader.h"

#include "Object3D.h"

#include "text2D.h"

void error_callback(int error, const char* description){
	fputs(description, stderr);
}


char text[256];
char text2[256];

float angleX = 0.0;
float angleY = 0.0;
float angleZ = 0.0;

glm::vec3 cameraPosition = glm::vec3(0, 5.0, 20.0);

Object3D dragon;
Object3D ballArray[5];
int i;

bool gameStatus = true;
bool isPaused = false;
bool gameEnd = false;


// Load music
#include <irrKlang.h>
using namespace irrklang;
ISoundEngine* SoundEngine = createIrrKlangDevice();
ISoundEngine* SoundEngine2 = createIrrKlangDevice();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
		switch (key)
		{
			// exit
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

			// start the game
		case GLFW_KEY_P:
			gameStatus = true;
			printf("\npress P ");
			break;

			// change camera
		case GLFW_KEY_A:
			cameraPosition.x += 1.0;
			break;
		case GLFW_KEY_Q:
			cameraPosition.x -= 1.0;
			break;
		case GLFW_KEY_S:
			cameraPosition.y += 1.0;
			break;
		case GLFW_KEY_W:
			cameraPosition.y -= 1.0;
			break;
		case GLFW_KEY_D:
			cameraPosition.z += 1.0;
			break;
		case GLFW_KEY_E:
			cameraPosition.z -= 1.0;
			break;

			// go to the right
		case GLFW_KEY_RIGHT:
			if (dragon.position.x < 4.0) {
				SoundEngine->play2D("audio/righToleft.mp3", false);
				dragon.position.x += 1.0;
			}
			break;

			// go to the left
		case GLFW_KEY_LEFT:
			if (dragon.position.x > -4.0) {
				SoundEngine->play2D("audio/leftToright.mp3", false);
				dragon.position.x -= 1.0;
			}
			break;
			
			// jump
		case GLFW_KEY_SPACE:
			if (dragon.position.y < 4.0) {
				SoundEngine->play2D("audio/jump.mp3", false);
				dragon.position.y += 1;
			}
			break;

		default:
			break;
		
		}
}
#include "windows.h" 


// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;

GLuint programIDLight;


//Light
glm::vec3 lightPosition(5.0f, 5.0f, 5.0f);
glm::vec3 lightColour(0.8f, 0.9f, 0.8f);

GLfloat lightRotation = -0.001f;

//Render objects
void renderObject(glm::vec3 colour, TextureLoader &texture)
{
	//Move light source
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, lightRotation, glm::vec3(1.0f, 1.0f, 0.0f));
	lightPosition = glm::vec3(rotationMat * glm::vec4(lightPosition, 1.0));

	//Lighting
	glUniform3f(glGetUniformLocation(programIDLight, "uObjectColour"), colour.x, colour.y, colour.z);
	glUniform3f(glGetUniformLocation(programIDLight, "uLightColour"), lightColour.x, lightColour.y, lightColour.z);
	glUniform3f(glGetUniformLocation(programIDLight, "uLightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(glGetUniformLocation(programIDLight, "uViewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//Model/View/Projection/Colour
	glUniformMatrix4fv(glGetUniformLocation(programIDLight, "uModel"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programIDLight, "uView"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programIDLight, "uProjection"), 1, GL_FALSE, &Projection[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture.texture);
}


#include<time.h>
const double NUM_SECONDS = 0.1;

int heart_counter = 3;

/*
After each collision the dragon is reborn if the hearts are still there.
*/
void reborn() {

	// if the number of hearts is less than 1, the game ends
	if (heart_counter < 1) {
		SoundEngine->play2D("audio/gameover.mp3", false);
		gameEnd = true;
	}

	// The ball will run from a random place.
	srand(static_cast <unsigned> (time(0)));
	dragon.position.x = 0;
	dragon.position.y += 3.8;
	for (i = 0; i < 5; i++) {
		float r = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - 0.1f)));
		ballArray[i].position = glm::vec3(2.0f * i - 4.0f, 0.85, -15 + r * -10.0f);
	}
	// The game is on pause
	// The player have to press the P key to continue the game.
	gameStatus = false;
}


int main( void ){

	// Main Song of the game
	SoundEngine->play2D("audio/daydream.mp3", true);


	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);

	// Open a window and create its OpenGL context
	// glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
	GLFWwindow* window = glfwCreateWindow(1024, // Breite
		768,  // Hoehe
		"CG - Tutorial", // Ueberschrift
		NULL,  // windowed mode
		NULL); // shared windoe

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current (wird nicht automatisch gemacht)
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// GLEW ermöglicht Zugriff auf OpenGL-API > 1.1
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	//Start the rendering process
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create and compile our GLSL program from the shaders

	programIDLight = LoadShaders("lightProjection.vert", "lightProjection.frag");

	// Shader auch benutzen !
	glUseProgram(programIDLight);

	// Set End game
	initText2D("Holstein.DDS");


	// Set ground
	Object3D boden;
	boden.loadASSIMP("resources/cube.obj");
	boden.setBuffers();
	boden.color = glm::vec3(0.54, 0.3, 0.22);
	boden.texture.load("resources/white.bmp");
	boden.texture.setBuffers();

	// Set  walls
	Object3D wall;
	wall.loadASSIMP("resources/cube.obj");
	wall.setBuffers();
	wall.color = glm::vec3(0.5, 1.0, 0.0);
	wall.texture.load("resources/greengras2.bmp");
	wall.texture.setBuffers();

	// Set ballArray
	srand(static_cast <unsigned> (time(0)));

	for (i = 0; i < 5; i++) {
		ballArray[i].loadASSIMP("resources/sphere.obj");
		ballArray[i].setBuffers();
		ballArray[i].color = glm::vec3(1, 1, 0.0);
		ballArray[i].texture.load("resources/newball.png");
	
		float r = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - 0.1f)));
		ballArray[i].position = glm::vec3(2.0f * i - 4.0f, 0.85, -10 + r * -10.0f);
		ballArray[i].texture.setBuffers();
		ballArray[i].rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		ballArray[i].speed = 0.5f;
	}


	//Set dragon
	dragon.position = glm::vec3(0, 1.5, 10);
	dragon.loadASSIMP("resources/dragon.obj");
	dragon.color = glm::vec3(1.0, 0.3, 0.0);
	dragon.setBuffers();
	dragon.texture.load("resources/white.bmp");
	dragon.texture.setBuffers();
	dragon.rotation = glm::vec4(12.0f, 0.0f, 0.0f, 1.0f);

	// Set time
	int count = 1;
	double time_counter1 = 0;
	double time_counter2 = 0;
	double time_counter3 = 0;
	int played_time_counter = 0;
	clock_t this_time = clock();
	clock_t last_time = this_time;


	//Set heart
	// The player has 3 hearts.
	// When all hearts are lost, the game ends
	Object3D heartArray[3];
	for (i = 0; i < 3; i++) {
		heartArray[i].position = glm::vec3(-4+1.3*i, 8, -10);
		heartArray[i].loadASSIMP("resources/heart.obj");
		heartArray[i].color = glm::vec3(0.8, 0, 0);
		heartArray[i].scale = glm::vec3(0.05, 0.05, 0.05);
		heartArray[i].rotation = glm::vec4(-90, 1, 0, 0);
		heartArray[i].setBuffers();
		heartArray[i].texture.load("resources/white.bmp");
		heartArray[i].texture.setBuffers();
	}
	
	// Firstly set the game status to false. Player press P to start the game
	gameStatus = false;
	bool isGESoungPlayed = false;

	// Eventloop
	while (!glfwWindowShouldClose(window) ) {

		// Play the gameover song if the game ends.
		if (gameEnd && !isGESoungPlayed) {
			printf("game over sound");
			SoundEngine2->play2D("audio/gameover.mp3", false);
			isGESoungPlayed = true;
		}

			// Time counting
			this_time = clock();

			time_counter1 += (double)(this_time - last_time);
			time_counter2 += (double)(this_time - last_time);
			time_counter3 += (double)(this_time - last_time);

			last_time = this_time;
			// Per 0.1 second
			// The position of each ball is changed every 0.1 seconds.
			if ( time_counter1 > (double)(NUM_SECONDS * CLOCKS_PER_SEC))
			{
				time_counter1 -= (double)(NUM_SECONDS * CLOCKS_PER_SEC);
				if (gameStatus) {
					for (i = 0; i < 5; i++) {
						ballArray[i].position.z += ballArray[i].speed;
						// the balls spins
						ballArray[i].rotation.x += 10.0f;
					}
				}
			}

			// Per 1.0 second
			if (time_counter2 > (double)(1.0 * CLOCKS_PER_SEC))
			{
				time_counter2 -= (double)(1.0 * CLOCKS_PER_SEC);
				if (gameStatus) {
					dragon.rotation.x *= -1.0;
					dragon.position.y = 1.5;
					played_time_counter += 1;
				}            
				
			}

			// Per 10.0 seconds
			// The speed of the balls will increase by 1.35 times every 10 seconds.
			if (time_counter3 > (double)(10.0 * CLOCKS_PER_SEC))
			{
				time_counter3 -= (double)(10.0 * CLOCKS_PER_SEC);
				for (i = 0; i < 5; i++) {
					ballArray[i].speed *= 1.35;
				}
			}

			// Check collision ball array
			if (dragon.position.y == 1.5) {
				for (i = 0; i < 5; i++) {
					if ((fabs(ballArray[i].position.z - dragon.position.z) < 1.0f)
						&& fabs(dragon.position.x - ballArray[i].position.x) < 1.0f
						&& ballArray[i].position.z < 11.0f)
					{
						// If the collision exists
						heart_counter -= 1;
						SoundEngine->play2D("audio/dead.mp3", false);
						reborn();
						break;

					}
				}
			}

			// the balls come back to the starting point
			for (i = 0; i < 5; i++) {
				if (ballArray[i].position.z > 16.0f)
				{
					float r = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - 0.1f)));
					ballArray[i].position.z = r * -10.0f;
				}
			}


			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT);

			// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
			Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

			// Camera matrix
			// Change camera location
			View = glm::lookAt(cameraPosition, // Camera is at (0,0,-5), in World Space
				glm::vec3(0, 0, 0),  // and looks at the origin
				glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down)

									 // Model matrix : an identity matrix (model will be at the origin)

			Model = glm::mat4(1.0f);

			//---- Keyboard Event- Change location 
			Model = glm::rotate(Model, angleX, glm::vec3(1.0, 0.0, 0.0));
			Model = glm::rotate(Model, angleY, glm::vec3(0.0, 1.0, 0.0));
			Model = glm::rotate(Model, angleZ, glm::vec3(0.0, 0.0, 1.0));


			//-------- Render  the ground
			glm::mat4 SaveBoden = Model;
			Model = glm::scale(Model, glm::vec3(6.0, 0.5, 27.0)); 
			Model = glm::translate(Model, glm::vec3(0.0, -0.25, 0.0));
			renderObject(boden.color, boden.texture);
			boden.render();
			Model = SaveBoden;

			//----- Render the left wall
			glm::mat4 SaveWall = Model;
			Model = glm::scale(Model, glm::vec3(0.5, 5, 27)); 
			Model = glm::translate(Model, glm::vec3(-12, 1, 0.0));
			renderObject(wall.color, wall.texture);
			wall.render();
			Model = SaveWall;

			//----- Render the right wall
			glm::mat4 SaveWall2 = Model;
			Model = glm::scale(Model, glm::vec3(0.5, 5, 27));
			Model = glm::translate(Model, glm::vec3(12, 1, 0.0));
			renderObject(wall.color, wall.texture);
			wall.render();
			Model = SaveWall2;

			// Render the dragon
			glm::mat4 SaveDragon = Model;
			Model = glm::translate(Model, glm::vec3(dragon.position.x, dragon.position.y, dragon.position.z));
			Model = glm::rotate(Model, 180.0f, glm::vec3(0.0, 1.0, 0.0));
			Model = glm::scale(Model, dragon.scale);
			Model = glm::rotate(Model, dragon.rotation.x, glm::vec3(dragon.rotation.y, dragon.rotation.z, dragon.rotation.w));

			renderObject(dragon.color, dragon.texture);
			dragon.render();
			Model = SaveDragon;

			// Render the balls
			for (i = 0; i < 5; i++) {
				glm::mat4 SaveBallArray = Model;
				Model = glm::translate(Model, glm::vec3(ballArray[i].position.x, ballArray[i].position.y, ballArray[i].position.z));
				Model = glm::rotate(Model, ballArray[i].rotation.x, glm::vec3(ballArray[i].rotation.y, ballArray[i].rotation.z, ballArray[i].rotation.w));
				renderObject(ballArray[i].color, ballArray[i].texture);
				ballArray[i].render();
				Model = SaveBallArray;
			}


			// Render  the hearts
			for (i = 0; i < heart_counter; i++) {
				glm::mat4 SaveHeart = Model;
				Model = glm::translate(Model, glm::vec3(heartArray[i].position.x, heartArray[i].position.y, heartArray[i].position.z));
				Model = glm::scale(Model, heartArray[i].scale);
				Model = glm::rotate(Model, heartArray[i].rotation.x, glm::vec3(heartArray[i].rotation.y, heartArray[i].rotation.z, heartArray[i].rotation.w));
				renderObject(heartArray[i].color, heartArray[i].texture);
				heartArray[i].render();
				Model = SaveHeart;
			}

			// when the game ends, comes to the result screen.
			if (gameEnd) {
				SoundEngine->stopAllSounds();
				sprintf(text, "End Game");
				printText2D(text, 200, 300, 60);
				sprintf(text2, "( %ds )", played_time_counter);
				printText2D(text2, 200, 200, 60);
			}

			// Swap buffers
			glfwSwapBuffers(window);

			// Poll for and process events
			glfwPollEvents();
		
    	}


	glDeleteProgram(programIDLight);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();


	return 0;

}
