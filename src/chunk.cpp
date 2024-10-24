#include "hFiles/chunk.h"
#include "hFiles/block.h"
#include <algorithm>
#include <glm/ext/vector_float2.hpp>

Chunk::Chunk(): active(false), loaded(false), offset(glm::vec2(0.0f, 0.0f)){
  data.resize(WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK);
}
void Chunk::Render(CubeRenderer &cubeRenderer){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int y = 0; y < HEIGHT_CHUNK; y++) {
      for (int z = 0; z < WIDTH_CHUNK; z++) {
        if (this->get(x, y, z).active)
          cubeRenderer.Render(glm::vec3(x+offset.x, y, z+offset.y), this->get(x,y,z));
      }
    }
  }
}

void Chunk::AddBlock(glm::vec3 position){
  int x = position.x; int y = position.y; int z = position.z;
  //std::cout << "pos: " << x << " | " << y << " | " << z << std::endl;
  this->setActive(x, y, z, true);
  bool new_faces[] = {
    z+1 >= WIDTH_CHUNK || !this->get(x,y,z+1).active,
    (z-1 < 0 || !this->get(x,y,z-1).active),
    (x-1 < 0 || !this->get(x-1, y, z).active),
    (x+1 >= WIDTH_CHUNK || !this->get(x+1, y, z).active),
    (y+1 >= HEIGHT_CHUNK || !this->get(x, y+1, z).active),
    (y-1 < 0 || !this->get(x, y-1, z).active)
  };
  this->setFaces(x, y, z, new_faces);

  if (x-1 >= 0) this->setFace(x-1, y, z, 3, false);
  if (x+1 < WIDTH_CHUNK) this->setFace(x+1,y, z, 2, false);
  if (y-1 >= 0) this->setFace(x, y-1, z, 4, false);
  if (y+1 < HEIGHT_CHUNK) this->setFace(x, y+1, z, 5, false);
  if (z-1 >= 0) this->setFace(x, y, z-1, 0, false);
  if (z+1 < WIDTH_CHUNK) this->setFace(x, y, z+1, 1, false);
}
void Chunk::RemoveBlock(glm::vec3 position){
  int x = position.x; int y = position.y; int z = position.z;
  std::cout << x << " | " << y << " | " << z << std::endl;
  this->setActive(x, y, z, false);
  if (x-1 >= 0) this->setFace(x-1, y, z, 3, true);
  if (x+1 < WIDTH_CHUNK) this->setFace(x+1, y, z, 2, true);
  if (y-1 >= 0) this->setFace(x, y-1, z, 4, true);
  if (y+1 < HEIGHT_CHUNK) this->setFace(x, y+1, z, 5, true);
  if (z-1 >= 0) this->setFace(x, y, z-1, 0, true);
  if (z+1 < WIDTH_CHUNK) this->setFace(x, y, z+1, 1, true);
}


void Chunk::InitChunk(const siv::PerlinNoise &perlin, glm::vec2 offset){
  int A = 0;
  this->offset = offset;
  this->MakeChunkData(perlin);
  this->loaded = true;
}

void Chunk::MakeChunkData(const siv::PerlinNoise &perlin){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      float maxY = (int) (HEIGHT_CHUNK*perlin.octave2D_01(((float)(x+offset.x) * 0.05), ((float)(z+offset.y) * 0.05), 4));
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        if (y <= maxY){
          this->setType(x, y, z, STONE);
          if (y > maxY-4) this->setType(x, y, z, DIRT);
          if (y == maxY) this->setType(x, y, z, GRASS);
          this->setActive(x, y, z, true);
          //std::fill(this->setType(x, y, z, STONE.faces, data[x][y][z].faces+6, 1);
        }
      }
    }
  }

  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        bool new_faces[] = {
          z+1 >= WIDTH_CHUNK || !this->get(x,y,z+1).active,
          (z-1 < 0 || !this->get(x,y,z-1).active),
          (x-1 < 0 || !this->get(x-1, y, z).active),
          (x+1 >= WIDTH_CHUNK || !this->get(x+1, y, z).active),
          (y+1 >= HEIGHT_CHUNK || !this->get(x, y+1, z).active),
          (y-1 < 0 || !this->get(x, y-1, z).active)
        };
        this->setFaces(x, y, z, new_faces);
      }
    }
  }
}


Block Chunk::get(int x, int y, int z){
  return data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*HEIGHT_CHUNK + x];
}
Block Chunk::get(glm::vec3 vec){
  return data[(int)vec.z*HEIGHT_CHUNK*WIDTH_CHUNK + (int)vec.y*HEIGHT_CHUNK + (int)vec.x];
}
void Chunk::setActive(int x, int y, int z, bool active){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*HEIGHT_CHUNK + x].active = true;
}
void Chunk::setFaces(int x, int y, int z, bool faces[]){
  for (int i = 0; i < 6; i++) 
      data[z * HEIGHT_CHUNK * WIDTH_CHUNK + y * HEIGHT_CHUNK + x].faces[i] = faces[i];
}
void Chunk::setFace(int x, int y, int z, int face, bool active){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*HEIGHT_CHUNK + x].faces[face] = active;
}
void Chunk::setType(int x, int y, int z, BLOCK_TYPE type){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*HEIGHT_CHUNK + x].type = type;
}

