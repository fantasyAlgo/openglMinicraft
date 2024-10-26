
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/ext/quaternion_geometric.hpp>
#include<iostream>

#include "hFiles/camera.h"
#include "hFiles/chunk.h"
#include "hFiles/cube_renderer.h"
#include "hFiles/resource_manager.h"
#include <cmath>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <PerlinNoise/PerlinNoise.hpp>
#include <vector>
#include "hFiles/block.h"
#include "hFiles/UI.h"


constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
// This is the number of chunks in the map
constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 20;

float deltaTime, lastFrame;

bool isInside(int i, int j);
void processInput(GLFWwindow *window);

int main(){
  const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

  //std::vector<Block> cubePositions;
  //Chunk chunk1 = Chunk();
  std::vector<std::vector<Chunk>> map(MAP_WIDTH);
  //Chunk map[MAP_WIDTH][MAP_HEIGHT];
  for (int x = 0; x < MAP_WIDTH; x++) 
    map[x].resize(MAP_HEIGHT);

  int current_chunk_x = MAP_WIDTH/2;
  int current_chunk_y = MAP_HEIGHT/2;

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
  camera.position = glm::vec3(MAP_WIDTH*WIDTH_CHUNK/4 - WIDTH_CHUNK/2, HEIGHT_CHUNK-3, MAP_HEIGHT*WIDTH_CHUNK/4 - WIDTH_CHUNK/2);

  glEnable(GL_DEPTH_TEST);

  glCullFace(GL_FRONT);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glm::vec3 cameraPointer;
  Block pointer_block;

  glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(90.0f), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
  ResourceManager::GetShader("main_shader").SetMatrix4("proj", proj, true);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Main while loop
  bool facesDye[] = {1,1,1,  1,1,1};
  Block noBlock = Block();
  noBlock.type = END_BLOCK;
  std::fill(noBlock.faces, noBlock.faces+6, 1);

  current_chunk_x = camera.position.x/WIDTH_CHUNK; current_chunk_y = camera.position.z/WIDTH_CHUNK;
  //std::cout << current_chunk_x << " | " << current_chunk_y << std::endl;

  //map[current_chunk_x][current_chunk_y].InitChunk(perlin, glm::vec2(current_chunk_x*WIDTH_CHUNK, current_chunk_y*WIDTH_CHUNK), );

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");


	while (!glfwWindowShouldClose(window)){
    current_chunk_x = camera.position.x/WIDTH_CHUNK; current_chunk_y = camera.position.z/WIDTH_CHUNK;
    //std::cout << camera.position.x << " | " << camera.position.z << std::endl;
    float currentFrame = glfwGetTime();
    camera.updatePointer(map[current_chunk_x][current_chunk_y]);
    //std::cout << camera.pointer_block.first.x
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);
    camera.keyboardHandling(window, deltaTime);
    /*camera.mouseHandling(window,
                         camera.active_pointer_block ? map[current_chunk_x][current_chunk_y] : map[(int) camera.pointer_block.chunk_pos.x][(int)camera.pointer_block.chunk_pos.y], 
                         deltaTime);*/
    camera.mouseHandling(window, map[current_chunk_x][current_chunk_y], deltaTime);
    //radius += 0.2;
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ResourceManager::GetShader("main_shader").SetMatrix4("view", camera.CameraLookAt(), true);
    //if (camera.active_pointer_block)
    //  cubeRenderer.Render(camera.pointer_block.first +glm::vec3(map[current_chunk_x][current_chunk_y].offset.x, 0, map[current_chunk_x][current_chunk_y].offset.y) + 0.0f, noBlock);
    
    camera.active_pointer_block = false;
    for (int i = -1; i < 2; i++){
      for (int j = -1; j < 2; j++){
        if (isInside(current_chunk_x+i, current_chunk_y+j) && !map[current_chunk_x+i][current_chunk_y+j].isLoaded){
          map[current_chunk_x+i][current_chunk_y+j].InitChunk(perlin, glm::vec2(current_chunk_x+i, current_chunk_y+j),
                                                            &map[current_chunk_x+i][current_chunk_y+j+1], &map[current_chunk_x+i][current_chunk_y+j-1],
                                                            &map[current_chunk_x+i-1][current_chunk_y+j], &map[current_chunk_x+i+1][current_chunk_y+j]);
        }
        if (isInside(current_chunk_x+i, current_chunk_y+j) && 
          (glm::dot(glm::vec2(i, j), glm::vec2(camera.direction.x, camera.direction.z)) > -0.2 || (i == 0 && j == 0))){
          //if (!camera.active_pointer_block) camera.updatePointer(map[current_chunk_x+i][current_chunk_y+j]);
          map[current_chunk_x+i][current_chunk_y+j].updateFaces();
          map[current_chunk_x+i][current_chunk_y+j].Render(cubeRenderer);
        }
      }
    }
  
    cameraPointer = glm::vec3((int)cameraPointer.x, (int)cameraPointer.y + 1, (int)cameraPointer.z);

    //UI::RenderUI();

    UI::RenderUI(WIDTH, HEIGHT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}
bool isInside(int i, int j){
  return i >= 0 && i < MAP_WIDTH &&
         j >= 0 && j < MAP_HEIGHT;
}


