#include "hFiles/chunk.h"
#include "hFiles/block.h"
#include <algorithm>
#include <glm/ext/vector_float2.hpp>

Chunk::Chunk(): active(false), isLoaded(false), offset(glm::vec2(0.0f, 0.0f)){
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
    !this->getActive(x, y, z+1),
    !this->getActive(x, y, z-1),
    !this->getActive(x-1, y, z),
    !this->getActive(x+1, y, z),
    !this->getActive(x, y+1, z),
    !this->getActive(x, y-1, z)
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
  //std::cout << x << " | " << y << " | " << z << std::endl;
  this->setActive(x, y, z, false);
  if (x-1 >= 0) this->setFace(x-1, y, z, 3, true);
  if (x+1 < WIDTH_CHUNK) this->setFace(x+1, y, z, 2, true);
  if (y-1 >= 0) this->setFace(x, y-1, z, 4, true);
  if (y+1 < HEIGHT_CHUNK) this->setFace(x, y+1, z, 5, true);
  if (z-1 >= 0) this->setFace(x, y, z-1, 0, true);
  if (z+1 < WIDTH_CHUNK) this->setFace(x, y, z+1, 1, true);
}


void Chunk::InitChunk(const siv::PerlinNoise &perlin, glm::vec2 offset, Chunk *up, Chunk *down, Chunk *left, Chunk *right){
  this->upChunk = up;
  this->bottomChunk = down;
  this->leftChunk = left;
  this->rightChunk = right;

  this->offset = offset;
  this->MakeChunkData(perlin);
  this->isLoaded = true;
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
  this->updateFaces();
}

void Chunk::updateFaces(){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      for (int y = 0; y < HEIGHT_CHUNK; y++) {
        bool new_faces[] = {
          !this->getActive(x, y, z+1),
          !this->getActive(x, y, z-1),
          !this->getActive(x-1, y, z),
          !this->getActive(x+1, y, z),
          !this->getActive(x, y+1, z),
          !this->getActive(x, y-1, z)
        };
        this->setFaces(x, y, z, new_faces);

      }
    }
  }
}

bool Chunk::getActive(int x, int y, int z){
  if (x < 0) return this->leftChunk->isLoaded ? this->leftChunk->getActive(WIDTH_CHUNK-1, y, z): true;
  if (x >= WIDTH_CHUNK) return this->rightChunk->isLoaded ? this->rightChunk->getActive(0, y, z): true;
  if (z < 0) return this->bottomChunk->isLoaded ? this->bottomChunk->getActive(x, y, WIDTH_CHUNK-1): true;
  if (z >= WIDTH_CHUNK) return this->upChunk->isLoaded ? this->upChunk->getActive(x, y, 0): true;

  if (y < 0) return true;
  if (y >= HEIGHT_CHUNK) return false;

  return data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].active;
}

Block Chunk::get(int x, int y, int z){
  Block block; block.active = false;
  if (x < 0 || y < 0 || z < 0) return block;
  if (x >= WIDTH_CHUNK || y >= HEIGHT_CHUNK || z >= WIDTH_CHUNK) return block;

  return data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x];
}
Block Chunk::get(glm::vec3 vec){
  return data[(int)vec.z*HEIGHT_CHUNK*WIDTH_CHUNK + (int)vec.y*WIDTH_CHUNK + (int)vec.x];
}
void Chunk::setActive(int x, int y, int z, bool active){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].active = true;
}
void Chunk::setFaces(int x, int y, int z, bool faces[]){
  for (int i = 0; i < 6; i++) 
      data[z * HEIGHT_CHUNK * WIDTH_CHUNK + y * WIDTH_CHUNK + x].faces[i] = faces[i];
}
void Chunk::setFace(int x, int y, int z, int face, bool active){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].faces[face] = active;
}
void Chunk::setType(int x, int y, int z, BLOCK_TYPE type){
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].type = type;
}

