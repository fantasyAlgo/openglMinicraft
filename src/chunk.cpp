#include "hFiles/chunk.h"
#include "hFiles/block.h"
#include <algorithm>
#include <glm/ext/vector_float2.hpp>

Chunk::Chunk(): active(false), loaded(false), offset(glm::vec2(0.0f, 0.0f)){
}
void Chunk::Render(CubeRenderer &cubeRenderer){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        if (data[x][y][z].active)
          cubeRenderer.Render(glm::vec3(x, y, z), data[x][y][z]);
      }
    }
  }
}

void Chunk::AddBlock(glm::vec3 position){
  int x = position.x; int y = position.y; int z = position.z;
  data[x][y][z].active = true;
  data[x][y][z].faces[0] = z+1 >= WIDTH_CHUNK || !data[x][y][z+1].active;
  data[x][y][z].faces[1] = (z-1 < 0 || !data[x][y][z-1].active);
  data[x][y][z].faces[2] = (x-1 < 0 || !data[x-1][y][z].active);
  data[x][y][z].faces[3] = (x+1 >= WIDTH_CHUNK || !data[x+1][y][z].active);
  data[x][y][z].faces[4] = (y+1 >= HEIGHT_CHUNK || !data[x][y+1][z].active);
  data[x][y][z].faces[5] = (y-1 < 0 || !data[x][y-1][z].active);
  if (x-1 >= 0) data[x-1][y][z].faces[3] = false;
  if (x+1 < WIDTH_CHUNK) data[x+1][y][z].faces[2] = false;
  if (y-1 >= 0) data[x][y-1][z].faces[4] = false;
  if (y+1 < HEIGHT_CHUNK) data[x][y+1][z].faces[5] = false;
  if (z-1 >= 0) data[x][y][z-1].faces[0] = false;
  if (z+1 < WIDTH_CHUNK) data[x][y][z+1].faces[1] = false;
}
void Chunk::RemoveBlock(glm::vec3 position){
  int x = position.x; int y = position.y; int z = position.z;
  data[x][y][z].active = false;
  if (x-1 >= 0) data[x-1][y][z].faces[3] = true;
  if (x+1 < WIDTH_CHUNK) data[x+1][y][z].faces[2] = true;
  if (y-1 >= 0) data[x][y-1][z].faces[4] = true;
  if (y+1 < HEIGHT_CHUNK) data[x][y+1][z].faces[5] = true;
  if (z-1 >= 0) data[x][y][z-1].faces[0] = true;
  if (z+1 < WIDTH_CHUNK) data[x][y][z+1].faces[1] = true;
}


void Chunk::InitChunk(const siv::PerlinNoise &perlin, glm::vec2 offset){
  std::cout << "banana" << std::endl;
  this->offset = offset;
  this->MakeChunkData(perlin);
  this->loaded = true;
}

void Chunk::MakeChunkData(const siv::PerlinNoise &perlin){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      float maxY = (int) (HEIGHT_CHUNK*perlin.octave2D_01(((float)(x+offset.x) * 0.01), ((float)(z+offset.y) * 0.01), 4));
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        if (y <= maxY){
          data[x][y][z].type = STONE;
          if (y > maxY-4) data[x][y][z].type = DIRT;
          if (y == maxY) data[x][y][z].type = GRASS;
          data[x][y][z].active = 1;
          //std::fill(data[x][y][z].faces, data[x][y][z].faces+6, 1);
        }
      }
    }
  }

  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        //if (!data[x][y][z].active) continue;
        data[x][y][z].faces[0] = z+1 >= WIDTH_CHUNK || !data[x][y][z+1].active;
        data[x][y][z].faces[1] = (z-1 < 0 || !data[x][y][z-1].active);
        data[x][y][z].faces[2] = (x-1 < 0 || !data[x-1][y][z].active);
        data[x][y][z].faces[3] = (x+1 >= WIDTH_CHUNK || !data[x+1][y][z].active);
        data[x][y][z].faces[4] = (y+1 >= HEIGHT_CHUNK || !data[x][y+1][z].active);
        data[x][y][z].faces[5] = (y-1 < 0 || !data[x][y-1][z].active);
      }
    }
  }
}




