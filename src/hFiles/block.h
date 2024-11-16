#pragma once

#include <algorithm>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <map>

constexpr int WIDTH_CHUNK = 31;
constexpr int HEIGHT_CHUNK = 64;
constexpr int WATER_LEVEL = HEIGHT_CHUNK/3;
constexpr int MOUNTAINS_HEIGHT = 2*HEIGHT_CHUNK/3;
constexpr int SAND_LEVEL = 3;
constexpr int N_TYPES = 14;

enum BLOCK_TYPE {
  GRASS,
  DIRT,
  STONE,
  PLANKS,
  TORCH,
  MINED_STONE,
  SAND,
  TREE_BLOCK,
  LEAVES,
  WATER_BASIC,
  SNOW,
  FLOWER,
  GRASS_FLOWER,
  END_BLOCK
};

extern glm::vec2 type_position[N_TYPES][3];
extern std::map<int, int> pos_to_type;
extern bool isBillBoard[N_TYPES];
void initMapType();

typedef struct Block{
  bool active = false;
  bool faces[6] = {0,0,0,0,0,0};
  BLOCK_TYPE type;
  char light_value;
  Block() : active(false), type(GRASS), light_value(0) { std::fill(faces, faces+6, 0);}
} Block;

