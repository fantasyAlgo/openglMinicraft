#include "hFiles/Game.h"
#include "hFiles/Settings.h"
#include "hFiles/UI.h"
#include "hFiles/block.h"
#include "hFiles/resource_manager.h"
#include <GL/gl.h>

bool isInside(int i, int j);

Game::Game(){
  this->Init();
}
void Game::Init(){
  srand(1000);

  for (int i = 0; i < MAP_WIDTH; i++) 
    map.push_back({});
  for (int x = 0; x < MAP_WIDTH; x++) 
    map[x].resize(MAP_HEIGHT);

  ResourceManager::LoadShader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl", nullptr, "main_shader");
  ResourceManager::LoadTexture("src/Textures/blocksAtlas.png", true, "awesomeface");

  this->current_chunk_x = MAP_WIDTH/2;
  this->current_chunk_y = MAP_HEIGHT/2;
  this->camera.InitCamera(WIDTH, HEIGHT);
  shaderR = ResourceManager::GetShader("main_shader");
  textureR = ResourceManager::GetTexture("awesomeface");

  cubeRenderer.Init(shaderR, textureR);
  camera.position = glm::vec3(MAP_WIDTH*WIDTH_CHUNK/4 - WIDTH_CHUNK/2, HEIGHT_CHUNK/2, MAP_HEIGHT*WIDTH_CHUNK/4 - WIDTH_CHUNK/2);

  glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(90.0f), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
  Shader shader = ResourceManager::GetShader("main_shader");//.SetMatrix4("proj", proj, true);
  shader.SetMatrix4("proj", proj, true);
  

  current_chunk_x = camera.position.x/WIDTH_CHUNK; current_chunk_y = camera.position.z/WIDTH_CHUNK;
  camera.place_type = GRASS;
}
void Game::Update(float dt){
    current_chunk_x = camera.position.x/WIDTH_CHUNK; current_chunk_y = camera.position.z/WIDTH_CHUNK;
    camera.updatePointer(map[current_chunk_x][current_chunk_y]);
    ResourceManager::GetShader("main_shader").SetMatrix4("view", camera.CameraLookAt(), true);
    //camera.active_pointer_block = false;
}
void Game::Render(){
    for (int i = -1; i < 2; i++){
      for (int j = -1; j < 2; j++){
        if (isInside(current_chunk_x+i, current_chunk_y+j) && !map[current_chunk_x+i][current_chunk_y+j].isLoaded){
          map[current_chunk_x+i][current_chunk_y+j].InitChunk(perlin, glm::vec2(current_chunk_x+i, current_chunk_y+j),
                                                            isInside(current_chunk_x+i, current_chunk_y+j+1) ? &map[current_chunk_x+i][current_chunk_y+j+1] : nullptr, 
                                                            isInside(current_chunk_x+i, current_chunk_y+j-1) ? &map[current_chunk_x+i][current_chunk_y+j-1] : nullptr,
                                                            isInside(current_chunk_x+i-1, current_chunk_y+j) ? &map[current_chunk_x+i-1][current_chunk_y+j] : nullptr, 
                                                            isInside(current_chunk_x+i+1, current_chunk_y+j) ? &map[current_chunk_x+i+1][current_chunk_y+j] : nullptr);
        }
        if (isInside(current_chunk_x+i, current_chunk_y+j) && 
          (glm::dot(glm::vec2(i, j), glm::vec2(camera.direction.x, camera.direction.z)) > -0.5 || (i == 0 && j == 0))){
          map[current_chunk_x+i][current_chunk_y+j].updateFaces();
          map[current_chunk_x+i][current_chunk_y+j].Render(cubeRenderer);
        }
      }
    }
    UI::RenderUI(WIDTH, HEIGHT);
}

void Game::ProcessInput(GLFWwindow *window, float dt){
    camera.keyboardHandling(window, dt);
    camera.mouseHandling(window,
                         !camera.active_pointer_block ? map[current_chunk_x][current_chunk_y] : map[(int) camera.pointer_block.chunk_pos.x][(int)camera.pointer_block.chunk_pos.y], 
                         dt);

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  for (int i = 0; i < (int)END_BLOCK; i++) {
    if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS) camera.place_type = (BLOCK_TYPE) i;
  }
}


bool isInside(int i, int j){
  return i >= 0 && i < MAP_WIDTH &&
         j >= 0 && j < MAP_HEIGHT;
}

