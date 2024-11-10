#include "hFiles/Game.h"
#include "hFiles/Settings.h"
#include "hFiles/UI.h"
#include "hFiles/block.h"
#include "hFiles/chunk.h"
#include "hFiles/resource_manager.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iterator>
#include <ostream>
#include <string>
#include <thread>
#include <type_traits>
#include <filesystem>
#include <fstream>
#include <format>
namespace fs = std::filesystem;

bool isInside(int i, int j);
Game::Game(){
  this->Init();
}
void Game::Init(){
  is_running = true;
  state = GameState::HOME_PAGE;
  srand(1000);

  for (int i = 0; i < MAP_WIDTH; i++) 
    map.push_back({});
  for (int x = 0; x < MAP_WIDTH; x++) 
    map[x].resize(MAP_HEIGHT);
  //chunk.InitChunk(perlin, glm::vec2(0.0f, 0.0f), nullptr, nullptr, nullptr, nullptr);

  ResourceManager::LoadShader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl", nullptr, "main_shader");
  ResourceManager::LoadTexture("src/Assets/Textures/blocksAtlas.png", true, "awesomeface");

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

  for (const auto& entry : fs::directory_iterator("Worlds")) {
    if (fs::is_directory(entry.path())) 
      this->files_list.push_back(entry.path().string());
  }
}

void Game::Update(float dt){
  if (state != GameState::PLAY) return;
  current_chunk_x = player.camera.position.x/WIDTH_CHUNK; current_chunk_y = player.camera.position.z/WIDTH_CHUNK;
  player.camera.updatePointer(map[current_chunk_x][current_chunk_y]);
  ResourceManager::GetShader("main_shader").SetMatrix4("view", player.camera.CameraLookAt(), true);
    //camera.active_pointer_block = false;
}

void Game::Render(){
  if (state == GameState::PLAY) renderGame();
  if (state == GameState::HOME_PAGE) renderHome();
  if (state == GameState::WORLD_PANEL) renderWorldPanel();
}

void Game::renderWorldPanel(){
  ImVec2 bottomSize = ImVec2(WIDTH/2 , 50);
  float start_height = HEIGHT/8;
  float offset = 50;
  int world_list_size = this->files_list.size();
  // Set window to full screen
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
  
  // Set window flags to make it fixed and fullscreen
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
  ImGui::Begin("Fullscreen Window", nullptr, windowFlags);

  ImGui::SetCursorPos(ImVec2(WIDTH/2-WIDTH/4, start_height-offset));
  ImGui::SetWindowFontScale(3.0);
  ImGui::Text("Worlds list", ImVec2(WIDTH/8, 50));
  ImGui::SetWindowFontScale(1.0);
  ImGui::Separator();
  if (ImGui::TreeNode("Worlds"))
  {
    static bool selection[16] = { false };
    for (int i = 0; i < world_list_size; i++) {
      ImGui::SetNextItemAllowOverlap();
      if (ImGui::Selectable(this->files_list[i].c_str(), selection[i], ImGuiSelectableFlags_AllowDoubleClick)){
        if (ImGui::IsMouseDoubleClicked(0)){
          this->world_name = this->files_list[i];
          this->state = GameState::PLAY;
        }
      }
      ImGui::SameLine();
      std::string delete_label = "Delete##" + std::to_string(i);
      if (ImGui::SmallButton(delete_label.c_str())){
        fs::remove_all(this->files_list[i]);
        this->files_list.erase(this->files_list.begin() + i);
        i--;
      }
    }
    ImGui::TreePop();
  }
  static char buf1[16];
  ImGui::SetCursorPos(ImVec2(WIDTH/8, HEIGHT-start_height*2.5f));
  ImGui::InputText(" World name", buf1, IM_ARRAYSIZE(buf1));

  ImGui::SetCursorPos(ImVec2(WIDTH/4, HEIGHT-start_height*2.0f));
  if (ImGui::Button("Create", bottomSize)){
    fs::create_directories(("Worlds/" + (std::string)buf1).c_str());
    files_list.push_back((std::string)buf1);
  }

  ImGui::SetCursorPos(ImVec2(WIDTH/4, HEIGHT-start_height));
  if (ImGui::Button("Undo", bottomSize))
    this->state = GameState::HOME_PAGE;

  //ImGui::ShowDemoWindow();
  ImGui::End();
}
void Game::renderHome(){
  ImVec2 bottomSize = ImVec2(WIDTH/2 , 50);
  float start_height = HEIGHT/3;
  float offset = 50;
  // Set window to full screen
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
  
  // Set window flags to make it fixed and fullscreen
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
  ImGui::Begin("Fullscreen Window", nullptr, windowFlags);
  // Add UI elements here
  ImGui::SetCursorPos(ImVec2(WIDTH/2-WIDTH/4, start_height-offset));
  ImGui::SetWindowFontScale(3.0);
  ImGui::Text("Minecraft", ImVec2(WIDTH/8, 50));
  ImGui::SetWindowFontScale(1.5);
  ImGui::SetCursorPos(ImVec2(WIDTH/4, start_height+offset));
  if (ImGui::Button("Play", bottomSize)) 
    this->state = GameState::WORLD_PANEL;
  ImGui::SetCursorPos(ImVec2(WIDTH/4, start_height+offset*3));
  if (ImGui::Button("Exit", bottomSize))
    this->is_running = false;
  ImGui::End();
}

void Game::renderGame(){
  for (int i = -CHUNK_RAD/2; i <= CHUNK_RAD/2; i++)
    for (int j = -CHUNK_RAD/2; j <= CHUNK_RAD/2; j++)
      if (isInside(current_chunk_x+i, current_chunk_y+j) && 
        (glm::dot(glm::vec2(i, j), glm::vec2(player.camera.direction.x, player.camera.direction.z)) > -0.5 || (i == 0 && j == 0)) &&
          map[current_chunk_x+i][current_chunk_y+j].isLoaded){
        while (map[current_chunk_x+i][current_chunk_y+j].isUpdating); //std::cout << "whoops" << std::endl;
        map[current_chunk_x+i][current_chunk_y+j].Render(cubeRenderer);
      }
  for (int i = -CHUNK_RAD/2; i <= CHUNK_RAD/2; i++)
    for (int j = -CHUNK_RAD/2; j <= CHUNK_RAD/2; j++)
      if (isInside(current_chunk_x+i, current_chunk_y+j) && 
        (glm::dot(glm::vec2(i, j), glm::vec2(player.camera.direction.x, player.camera.direction.z)) > -0.5 || (i == 0 && j == 0)) && 
          map[current_chunk_x+i][current_chunk_y+j].isLoaded)
        //map[current_chunk_x+i][current_chunk_y+j].updateFaces();
        map[current_chunk_x+i][current_chunk_y+j].RenderWater(cubeRenderer);
  UI::RenderUI(WIDTH, HEIGHT, player.camera.direction);
  player.RenderInventory(this->textureR.ID);
}

void Game::ProcessInput(GLFWwindow *window, float dt){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  if (state != GameState::PLAY){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return;
  }

  player.camera.keyboardHandling(window, dt);
  player.camera.mouseHandling(window,
                       !player.camera.active_pointer_block ? map[current_chunk_x][current_chunk_y] : map[(int) player.camera.pointer_block.chunk_pos.x][(int)player.camera.pointer_block.chunk_pos.y], 
                       dt);

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
    if (!this->chunk_updates.empty()){
      this->map[(int)this->chunk_updates.front().x][(int)this->chunk_updates.front().y].update();
      this->chunk_updates.pop();
    }
    for (int i = -CHUNK_RAD/2; i < CHUNK_RAD/2; i++) {
      for (int j = -CHUNK_RAD/2; j < CHUNK_RAD/2; j++) {
        if (isInside(current_chunk_x+i, current_chunk_y+j) && (this->map[current_chunk_x+i][current_chunk_y+j].needsUpdate ||
          this->map[current_chunk_x+i][current_chunk_y+j].firstRendered)){
            this->map[current_chunk_x+i][current_chunk_y+j].update();
            this->chunk_updates.push(glm::vec2(current_chunk_x+i, current_chunk_y+j));
            this->map[current_chunk_x+i][current_chunk_y+j].needsUpdate = false;
            this->map[current_chunk_x+i][current_chunk_y+j].firstRendered = false;
        }
      }
    }
    if (this->state != GameState::PLAY) continue;
    // place the correct stuff
    if ((int)current_chunk.x != current_chunk_x || (int)current_chunk.y != current_chunk_y){
      current_chunk = glm::vec2(current_chunk_x, current_chunk_y);
      pos = current_chunk;
      currD = 0;
      dir = 0;
      dMax = 1;
    }
    if (abs(pos.x-current_chunk_x) >= CHUNK_RAD+2) continue;
    //std::cout << "Chunk " << pos.x << " " << pos.y << " loaded, with: " << currD << " " << dir << std::endl;
    if (isInside(pos.x, pos.y) && !this->map[(int)pos.x][(int)pos.y].isLoaded){
      this->map[(int)pos.x][(int)pos.y].InitChunk(perlin, this->world_name, pos,
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


void Game::saveWorld(){
  std::string name = this->world_name;
  if (!(name[0] == 'W' && name[1] == 'o' && name[2] == 'r' && name[3] == 'l' && name[4] == 'd'))
    name = "Worlds/" + name;
  std::string path;
  std::ofstream outputFileStream;
  for (int i = 0; i < MAP_WIDTH; i++) {
    for (int j = 0; j < MAP_HEIGHT; j++) {
      if (map[i][j].isLoaded){
        path = name + "/chunk" + std::to_string(i) + "_" + std::to_string(j) + ".dat";
        //std::cout << "path: " << path << std::endl;
        outputFileStream.open(path.c_str(), std::ios::out|std::ios::binary);
        outputFileStream.write((char*)map[i][j].all_packed_data, WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK*sizeof(unsigned char));
        outputFileStream.close();
      }
    }
  }
}












