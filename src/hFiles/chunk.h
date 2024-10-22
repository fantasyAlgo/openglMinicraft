#pragma once

#include "cube_renderer.h"
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <PerlinNoise/PerlinNoise.hpp>

constexpr int WIDTH_CHUNK = 16;
constexpr int HEIGHT_CHUNK = 16;

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

class Chunk {
public:
  Block data[WIDTH_CHUNK][HEIGHT_CHUNK][WIDTH_CHUNK];
  Chunk(const siv::PerlinNoise &perlin, std::string *fileName = nullptr);
  void MakeChunkData(const siv::PerlinNoise &perlin);
  void Render(CubeRenderer &cubeRenderer);

  void AddBlock(glm::vec3 position);
  void RemoveBlock(glm::vec3 position);

  bool isInside(glm::vec3 pos){
    return pos.x >= 0 && pos.x < WIDTH_CHUNK &&
           pos.y >= 0 && pos.y < HEIGHT_CHUNK &&
           pos.z >= 0 && pos.z < WIDTH_CHUNK;
  }
};
