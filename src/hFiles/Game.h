#pragma once

#include "Player.h"
//#include "camera.h"
#include "chunk.h"
#include "Settings.h"
#include <GLFW/glfw3.h>
#include <future>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <queue>
#include <string>
#include <vector>

enum class GameState {
  HOME_PAGE,
  WORLD_PANEL,
  PLAY
};


class Game {
private:
  bool isFullWindow = false;
  GameState state;
  int current_chunk_x, current_chunk_y;
  BLOCK_TYPE place_type = GRASS;
  glm::vec3 camera_pointer;
  std::vector<std::string> files_list;

	const siv::PerlinNoise perlin{ seed };
  std::vector<std::vector<Chunk>> map;
  Chunk chunk;

  Player player;
  //Camera camera;
  CubeRenderer cubeRenderer; //= CubeRenderer(shaderR, textureR);
  Shader shaderR; //= ResourceManager::GetShader("main_shader");
  Texture2D textureR; //= ResourceManager::GetTexture("awesomeface");
  std::future<bool> chunk_loader_thread;
  std::queue<glm::vec2> chunk_updates;

public:
  bool is_running;
  std::string world_name;
  Game();
  //~Game();
  void Init();
  void ProcessInput(GLFWwindow *window, float dt);
  void Update(float dt);
  bool ChunkLoader();
  void renderHome();
  void renderGame();
  void renderWorldPanel();
  void Render();

  void saveWorld();
};





