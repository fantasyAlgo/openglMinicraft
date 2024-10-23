#pragma once

#include "cube_renderer.h"
#include "block.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <PerlinNoise/PerlinNoise.hpp>

class Chunk {
public:
  glm::vec2 offset;
  bool active;
  bool loaded;


  Block data[WIDTH_CHUNK][HEIGHT_CHUNK][WIDTH_CHUNK];

  Chunk();
  void setOffset(glm::vec2 offset);
  void InitChunk(const siv::PerlinNoise &perlin, glm::vec2 offset);
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
