#pragma once

#include "Player.h"
//#include "camera.h"
#include "chunk.h"
#include "Settings.h"
#include <GLFW/glfw3.h>
#include <future>
#include <glm/ext/vector_float3.hpp>

class Game {
private:
  int current_chunk_x;
  int current_chunk_y;
  BLOCK_TYPE place_type = GRASS;
  glm::vec3 camera_pointer;

	const siv::PerlinNoise perlin{ seed };
  std::vector<std::vector<Chunk>> map;
  Chunk chunk;

  Player player;
  //Camera camera;
  CubeRenderer cubeRenderer; //= CubeRenderer(shaderR, textureR);
  Shader shaderR; //= ResourceManager::GetShader("main_shader");
  Texture2D textureR; //= ResourceManager::GetTexture("awesomeface");
  std::future<bool> chunk_loader_thread;


public:
  bool is_running;
  Game();
  //~Game();
  void Init();
  void ProcessInput(GLFWwindow *window, float dt);
  void Update(float dt);
  bool ChunkLoader();
  void Render();
};
