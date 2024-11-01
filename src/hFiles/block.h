#pragma once

#include <algorithm>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

constexpr int WIDTH_CHUNK = 32;
constexpr int HEIGHT_CHUNK = 64;
constexpr int WATER_LEVEL = HEIGHT_CHUNK/3;
constexpr int MOUNTAINS_HEIGHT = 2*HEIGHT_CHUNK/3;
constexpr int SAND_LEVEL = 3;

enum BLOCK_TYPE {
  GRASS,
  DIRT,
  STONE,
  MINED_STONE,
  SAND,
  TREE_BLOCK,
  LEAVES,
  WATER_BASIC,
  SNOW,
  END_BLOCK
};

extern glm::vec2 type_position[10][3];

typedef struct Block{
  bool active = false;
  bool faces[6] = {0,0,0,0,0,0};
  BLOCK_TYPE type;
  Block() : active(false), type(GRASS) { std::fill(faces, faces+6, 0);}
} Block;

