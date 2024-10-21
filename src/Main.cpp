#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>
#include <algorithm>
#include <type_traits>

#include "hFiles/camera.h"
#include "hFiles/cube_renderer.h"
#include "hFiles/resource_manager.h"
#include <cmath>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

#include "libs/PerlinNoise.hpp"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int WIDTH_BLOCKS = 50;
constexpr int HEIGHT_BLOCKS = 16;
/*
* 0 front
* 1 back
* 2 left 
* 3 right
* 4 top
* 5 bottom
* */
typedef struct Block{
  bool active = false;
  bool faces[6] = {0,0,0,0,0,0};
} Block;

float deltaTime, lastFrame;
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

int main(){
  const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

  std::vector<Block> cubePositions;
  Block chunck[WIDTH_BLOCKS][HEIGHT_BLOCKS][WIDTH_BLOCKS];
  for (int x = 0; x < WIDTH_BLOCKS; x++) {
    for (int z = 0; z < WIDTH_BLOCKS; z++) {
      float maxY = std::min(std::max((int) (40.0f*perlin.octave2D_01(((float)x * 0.01), ((float)z * 0.01), 4)) - 15, 0), 16);
      for (int y = 0; y < HEIGHT_BLOCKS; y++) {
        if (y < maxY){
          chunck[x][y][z].active = 1;
          std::fill(chunck[x][y][z].faces, chunck[x][y][z].faces+6, 0);
        }
      }
    }
  }
  for (int x = 0; x < WIDTH_BLOCKS; x++) {
    for (int z = 0; z < WIDTH_BLOCKS; z++) {
      for (int y = 0; y < HEIGHT_BLOCKS; y++) {
        if (!chunck[x][y][z].active) continue;
        chunck[x][y][z].faces[0] = z+1 >= WIDTH_BLOCKS || !chunck[x][y][z+1].active;
        chunck[x][y][z].faces[1] = (z-1 < 0 || !chunck[x][y][z-1].active);
        chunck[x][y][z].faces[2] = (x-1 < 0 || !chunck[x-1][y][z].active);
        chunck[x][y][z].faces[3] = (x+1 >= WIDTH_BLOCKS || !chunck[x+1][y][z].active);
        chunck[x][y][z].faces[4] = (y+1 >= HEIGHT_BLOCKS || !chunck[x][y+1][z].active);
        chunck[x][y][z].faces[5] = (y-1 < 0 || !chunck[x][y-1][z].active);
      }
    }
  }

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, false);
	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Breakout", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

  ResourceManager::LoadShader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl", nullptr, "main_shader");
  ResourceManager::LoadTexture("src/Textures/blocksAtlas.png", true, "awesomeface");
  Shader shaderR = ResourceManager::GetShader("main_shader");
  Texture2D textureR = ResourceManager::GetTexture("awesomeface");
  CubeRenderer cubeRenderer = CubeRenderer(shaderR, textureR);
  Camera camera(WIDTH, HEIGHT);

  glEnable(GL_DEPTH_TEST);

  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
  //glFrontFace(GL_CCW);



  glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(90.0f), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
  ResourceManager::GetShader("main_shader").SetMatrix4("proj", proj, true);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Main while loop
	while (!glfwWindowShouldClose(window)){
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);
    camera.inputHandling(window, deltaTime);
    //radius += 0.2;
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ResourceManager::GetShader("main_shader").SetMatrix4("view", camera.CameraLookAt(), true);
    
    for (int x = 0; x < WIDTH_BLOCKS; x++) {
      for (int z = 0; z < WIDTH_BLOCKS; z++) {
        for (int y = 0; y < HEIGHT_BLOCKS; y++) {
          if (chunck[x][y][z].active)
            cubeRenderer.Render(glm::vec3(x, y, z), chunck[x][y][z].faces);
        }
      }
    }

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

