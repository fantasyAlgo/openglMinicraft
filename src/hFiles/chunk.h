#pragma once

#include "cube_renderer.h"
#include "block.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <PerlinNoise/PerlinNoise.hpp>
#include <vector>
#include "Settings.h"

class Chunk {
public:
  glm::vec2 offset, map_pos;
  bool active, isLoaded, firstRendered, needsUpdate, isUpdating;

  //Block data[WIDTH_CHUNK][HEIGHT_CHUNK][WIDTH_CHUNK];
  std::vector<Block> data;
  int n_packed_data, n_packed_data_water, n_file_data, n_back_packed_data;
  int back_packed_data[WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK*6];
  int packed_data[WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK*6];
  int packed_data_water[WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK/2];
  unsigned char all_packed_data[WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK];
  Chunk *upChunk, *bottomChunk, *leftChunk, *rightChunk;

  Chunk();
  void setOffset(glm::vec2 offset);
  void InitChunk(const siv::PerlinNoise &perlin, std::string world_name, glm::vec2 offset, Chunk *up, Chunk *down, Chunk *left, Chunk *right);
  void LoadChunkFile(std::string path);

  void MakeTrees(int x, int startY, int z);
  void MakeChunkData(const siv::PerlinNoise &perlin);
  void LoadChunkData(std::string world_name);

  void Render(CubeRenderer &cubeRenderer);
  void RenderWater(CubeRenderer &cubeRenderer);

  // update calls updatePackedData and updateFaces;
  void update();
  void updatePackedData();
  //void updateFromPackedData();
  //void updateAllPacked();

  void updateFace(int x, int y, int z);
  void updateFaces();
  void AddBlock(glm::vec3 position, BLOCK_TYPE type);
  void RemoveBlock(glm::vec3 position);

  Block get(int x, int y, int z);
  Block get(glm::vec3 vec);
  bool getActive(int x, int y, int z);

  void setActive(int x, int y, int z, bool active);
  void setFaces(int x, int y, int z, bool faces[]);
  void setFace(int x, int y, int z, int face, bool active);
  void setType(int x, int y, int z, BLOCK_TYPE type);

  bool isInside(glm::vec3 pos){
    return pos.x >= 0 && pos.x < WIDTH_CHUNK &&
           pos.y >= 0 && pos.y < HEIGHT_CHUNK &&
           pos.z >= 0 && pos.z < WIDTH_CHUNK;
  } 

};

