#include "hFiles/Game.h"
#include "hFiles/Settings.h"
#include "hFiles/UI.h"
#include "hFiles/block.h"
#include "hFiles/chunk.h"
#include "hFiles/resource_manager.h"
#include <GL/gl.h>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iterator>
#include <thread>

bool isInside(int i, int j);

Game::Game(){
  this->Init();
}
void Game::Init(){
  is_running = true;
  srand(1000);

  for (int i = 0; i < MAP_WIDTH; i++) 
    map.push_back({});
  for (int x = 0; x < MAP_WIDTH; x++) 
    map[x].resize(MAP_HEIGHT);
  //chunk.InitChunk(perlin, glm::vec2(0.0f, 0.0f), nullptr, nullptr, nullptr, nullptr);

  ResourceManager::LoadShader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl", nullptr, "main_shader");
  ResourceManager::LoadTexture("src/Textures/blocksAtlas.png", true, "awesomeface");

  this->current_chunk_x = MAP_WIDTH/2;
  this->current_chunk_y = MAP_HEIGHT/2;
  shaderR = ResourceManager::GetShader("main_shader");
  textureR = ResourceManager::GetTexture("awesomeface");

  cubeRenderer.Init(shaderR, textureR);
  player.setPosition(glm::vec3(MAP_WIDTH*WIDTH_CHUNK/4 - WIDTH_CHUNK/2, HEIGHT_CHUNK/2, MAP_HEIGHT*WIDTH_CHUNK/4 - WIDTH_CHUNK/2));
  //camera.position = glm::vec3(0, 40, 0);

  glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(90.0f), (float)(WIDTH / HEIGHT), 0.1f, 110.0f);
  Shader shader = ResourceManager::GetShader("main_shader");//.SetMatrix4("proj", proj, true);
  shader.SetMatrix4("proj", proj, true);
  shader.SetVector3f("lightPos", glm::vec3(10, 10, 60));
  

  current_chunk_x = player.camera.position.x/WIDTH_CHUNK; current_chunk_y = player.camera.position.z/WIDTH_CHUNK;
  player.camera.place_type = GRASS;
  chunk_loader_thread = std::async(std::launch::async, &Game::ChunkLoader, this);
}
void Game::Update(float dt){
    current_chunk_x = player.camera.position.x/WIDTH_CHUNK; current_chunk_y = player.camera.position.z/WIDTH_CHUNK;
    player.camera.updatePointer(map[current_chunk_x][current_chunk_y]);
    ResourceManager::GetShader("main_shader").SetMatrix4("view", player.camera.CameraLookAt(), true);
    //camera.active_pointer_block = false;
}

void Game::Render(){
  for (int i = -CHUNK_RAD/2; i <= CHUNK_RAD/2; i++){
    for (int j = -CHUNK_RAD/2; j <= CHUNK_RAD/2; j++){
      /*if (isInside(current_chunk_x+i, current_chunk_y+j) && !map[current_chunk_x+i][current_chunk_y+j].isLoaded){
        map[current_chunk_x+i][current_chunk_y+j].InitChunk(perlin, glm::vec2(current_chunk_x+i, current_chunk_y+j),
                                                          isInside(current_chunk_x+i, current_chunk_y+j+1) ? &map[current_chunk_x+i][current_chunk_y+j+1] : nullptr, 
                                                          isInside(current_chunk_x+i, current_chunk_y+j-1) ? &map[current_chunk_x+i][current_chunk_y+j-1] : nullptr,
                                                          isInside(current_chunk_x+i-1, current_chunk_y+j) ? &map[current_chunk_x+i-1][current_chunk_y+j] : nullptr, 
                                                          isInside(current_chunk_x+i+1, current_chunk_y+j) ? &map[current_chunk_x+i+1][current_chunk_y+j] : nullptr);
      }*/
      if (isInside(current_chunk_x+i, current_chunk_y+j) && 
        (glm::dot(glm::vec2(i, j), glm::vec2(player.camera.direction.x, player.camera.direction.z)) > -0.5 || (i == 0 && j == 0)) &&
          map[current_chunk_x+i][current_chunk_y+j].isLoaded){
        map[current_chunk_x+i][current_chunk_y+j].Render(cubeRenderer);
      }
    }
  }
  for (int i = -CHUNK_RAD/2; i <= CHUNK_RAD/2; i++){
    for (int j = -CHUNK_RAD/2; j <= CHUNK_RAD/2; j++){
      if (isInside(current_chunk_x+i, current_chunk_y+j) && 
        (glm::dot(glm::vec2(i, j), glm::vec2(player.camera.direction.x, player.camera.direction.z)) > -0.5 || (i == 0 && j == 0)) && 
          map[current_chunk_x+i][current_chunk_y+j].isLoaded){
        //map[current_chunk_x+i][current_chunk_y+j].updateFaces();
        map[current_chunk_x+i][current_chunk_y+j].RenderWater(cubeRenderer);
      }
    }
  }

  UI::RenderUI(WIDTH, HEIGHT, player.camera.direction);
  player.RenderInventory(this->textureR.ID);
}

void Game::ProcessInput(GLFWwindow *window, float dt){
    player.camera.keyboardHandling(window, dt);
    player.camera.mouseHandling(window,
                         !player.camera.active_pointer_block ? map[current_chunk_x][current_chunk_y] : map[(int) player.camera.pointer_block.chunk_pos.x][(int)player.camera.pointer_block.chunk_pos.y], 
                         dt);

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  for (int i = 0; i < (int)END_BLOCK; i++) {
    if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS) player.setPickedItem(i);
  }
}


bool isInside(int i, int j){
  return i >= 0 && i < MAP_WIDTH &&
         j >= 0 && j < MAP_HEIGHT;
}



bool Game::ChunkLoader(){
  std::cout << "Chunk loader thread" << std::endl;
  glm::vec2 current_chunk = glm::vec2(current_chunk_x, current_chunk_y);
  glm::vec2 pos = current_chunk;
  glm::vec2 directions[] = {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, -1.0f), glm::vec2(-1.0f, 0.0f), glm::vec2(0.0f, 1.0f)};
  int dMax = 1;
  int currD = 0;
  int dir = 0;
  while (is_running){
    for (int i = -CHUNK_RAD/2; i <= CHUNK_RAD/2; i++){
      for (int j = -CHUNK_RAD/2; j <= CHUNK_RAD/2; j++){
        if (isInside(current_chunk_x+i, current_chunk_y+j) && map[current_chunk_x+i][current_chunk_y+j].isLoaded && map[current_chunk_x+i][current_chunk_y+j].needsUpdate){
          if (i != 0 || j != 0)
            map[current_chunk_x+i][current_chunk_y+j].updatePackedData();
          map[current_chunk_x+i][current_chunk_y+j].needsUpdate = false;
        }
      }
    }
    // place the correct stuff
    if ((int)current_chunk.x != current_chunk_x || (int)current_chunk.y != current_chunk_y){
      current_chunk = glm::vec2(current_chunk_x, current_chunk_y);
      pos = current_chunk;
      currD = 0;
      dir = 0;
      dMax = 1;
    }
    if (abs(pos.x-current_chunk_x) >= CHUNK_RAD+2) continue;
    std::cout << "Chunk " << pos.x << " " << pos.y << " loaded, with: " << currD << " " << dir << std::endl;
    if (isInside(pos.x, pos.y) && !this->map[(int)pos.x][(int)pos.y].isLoaded){
      this->map[(int)pos.x][(int)pos.y].InitChunk(perlin, pos,
                                                  isInside((int)pos.x, (int)pos.y+1) ? &map[(int)pos.x][(int)pos.y+1] : nullptr, 
                                                  isInside((int)pos.x, (int)pos.y-1) ? &map[(int)pos.x][(int)pos.y-1] : nullptr,
                                                  isInside((int)pos.x-1, (int)pos.y) ? &map[(int)pos.x-1][(int)pos.y] : nullptr, 
                                                  isInside((int)pos.x+1, (int)pos.y) ? &map[(int)pos.x+1][(int)pos.y] : nullptr);
    }
    pos += directions[dir%4];
    currD++;
    if (currD == dMax || currD == dMax*2)
      dir++;
    if (currD == dMax*2){
      currD = 0;
      dMax++;
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  return true;
}
