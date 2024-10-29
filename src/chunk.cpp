#include "hFiles/chunk.h"
#include "hFiles/block.h"
#include <algorithm>
#include <any>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>

Chunk::Chunk(): active(false), isLoaded(false), offset(glm::vec2(0.0f, 0.0f)){
  data.resize(WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK);
}
void Chunk::Render(CubeRenderer &cubeRenderer){
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int y = 0; y < HEIGHT_CHUNK; y++) {
      for (int z = 0; z < WIDTH_CHUNK; z++) {
        if (this->getActive(x, y, z))
          cubeRenderer.Render(glm::vec3(x+offset.x, y, z+offset.y) + 0.5f, this->get(x,y,z));
      }
    }
  }
}

void Chunk::AddBlock(glm::vec3 position, BLOCK_TYPE type){
  int x = position.x; int y = position.y; int z = position.z;
  //std::cout << "pos: " << x << " | " << y << " | " << z << std::endl;
  this->setActive(x, y, z, true);
  this->setType(x, y, z, type);
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
  this->setFace(x-1, y, z, 3, true);
  this->setFace(x+1, y, z, 2, true);
  this->setFace(x, y-1, z, 4, true);
  this->setFace(x, y+1, z, 5, true);
  this->setFace(x, y, z-1, 0, true);
  this->setFace(x, y, z+1, 1, true);
}


void Chunk::InitChunk(const siv::PerlinNoise &perlin, glm::vec2 chunk_pos, Chunk *up, Chunk *down, Chunk *left, Chunk *right){
  this->upChunk = up;
  this->bottomChunk = down;
  this->leftChunk = left;
  this->rightChunk = right;

  this->map_pos = chunk_pos;
  this->offset = (float)WIDTH_CHUNK*chunk_pos;
  this->MakeChunkData(perlin);
  this->isLoaded = true;
}


void Chunk::MakeTrees(int x, int startY, int z){
  int howTall = rand()%6+ 4;
  int width = 3;
  int heightLeaves = 3;
  for (int y = startY; y < (int)startY+howTall; y++) {
    this->setType(x, y, z, TREE_BLOCK);
    this->setActive(x, y, z, true);
  }
  for (int i = -width/2; i <= width/2; i++) {
    for (int j = -width/2 ; j <= width/2; j++) {
      for (int k = 0; k < heightLeaves; k++) {
        this->setType(x+i, startY+howTall+k, z+j, LEAVES);
        this->setActive(x+i, startY+howTall+k, z+j, true);
      }
    }
  }


}
void Chunk::MakeChunkData(const siv::PerlinNoise &perlin){
  float maxY, cavePerlin, treeY;
  cavePerlin = 1.0f;
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      maxY = (int) (HEIGHT_CHUNK*perlin.octave2D_01(((float)(x+offset.x) * 0.025), ((float)(z+offset.y) * 0.025), 4));
      if (maxY >= WATER_LEVEL+SAND_LEVEL && rand()/100000000.0f < 0.4)
        this->MakeTrees(x, maxY, z);
      for (int y = 0; y <= HEIGHT_CHUNK; y++) {
        //cavePerlin = perlin.octave2D_01(((float)(x+offset.x) * 0.04), ((float)(z+offset.y) * 0.04), y*0.1);
        //std::cout << cavePerlin << std::endl;
        if (maxY < WATER_LEVEL && y <= WATER_LEVEL){
          this->setType(x, y, z, WATER_BASIC);
          this->setActive(x, y, z, true);
        }
        if (y <= maxY && cavePerlin > 0.34){
          if (maxY >= WATER_LEVEL && maxY < WATER_LEVEL+SAND_LEVEL){
            this->setType(x, y, z, SAND);
            this->setActive(x, y, z, true);
          }else{
            this->setType(x, y, z, STONE);
            if (maxY > MOUNTAINS_HEIGHT){
              if (maxY > MOUNTAINS_HEIGHT+5 && y > maxY-2) this->setType(x, y, z, SNOW);
            }else{
              if (y > maxY-4) this->setType(x, y, z, DIRT);
              if (y == maxY) this->setType(x, y, z, GRASS);
            }
            this->setActive(x, y, z, true);
          }
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
  if (!this->isLoaded) return false;
  if (x < 0) return            this->leftChunk != nullptr && this->leftChunk->isLoaded ? this->leftChunk->getActive(WIDTH_CHUNK-1, y, z): true;
  if (x >= WIDTH_CHUNK) return this->rightChunk != nullptr && this->rightChunk->isLoaded ? this->rightChunk->getActive(0, y, z): true;
  if (z < 0) return            this->bottomChunk != nullptr && this->bottomChunk->isLoaded ? this->bottomChunk->getActive(x, y, WIDTH_CHUNK-1): true;
  if (z >= WIDTH_CHUNK) return this->upChunk != nullptr && this->upChunk->isLoaded ? this->upChunk->getActive(x, y, 0): true;

  if (y < 0) return true;
  if (y >= HEIGHT_CHUNK) return false;

  return data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].active;
}

Block Chunk::get(int x, int y, int z){
  Block block; block.active = true;
  if (x < 0) return this->leftChunk != nullptr && this->leftChunk->isLoaded ? this->leftChunk->get(WIDTH_CHUNK-1, y, z): block;
  if (x >= WIDTH_CHUNK) return this->rightChunk != nullptr && this->rightChunk->isLoaded ? this->rightChunk->get(0, y, z): block;
  if (z < 0) return this->bottomChunk != nullptr && this->bottomChunk->isLoaded ? this->bottomChunk->get(x, y, WIDTH_CHUNK-1): block;
  if (z >= WIDTH_CHUNK) return this->upChunk != nullptr && this->upChunk->isLoaded ? this->upChunk->get(x, y, 0): block;

  if (x < 0 || y < 0 || z < 0) return block;
  if (x >= WIDTH_CHUNK || y >= HEIGHT_CHUNK || z >= WIDTH_CHUNK) return block;

  return data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x];
}
Block Chunk::get(glm::vec3 vec){
  return data[(int)vec.z*HEIGHT_CHUNK*WIDTH_CHUNK + (int)vec.y*WIDTH_CHUNK + (int)vec.x];
}
void Chunk::setActive(int x, int y, int z, bool active){
  if (x < 0){
    if (this->leftChunk->isLoaded) this->leftChunk->setActive(WIDTH_CHUNK-1, y, z, active);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk->isLoaded) this->rightChunk->setActive(0, y, z, active);
    return;
  }
  if (z < 0){
    if (this->bottomChunk->isLoaded) this->bottomChunk->setActive(x, y, WIDTH_CHUNK-1, active);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk->isLoaded) this->upChunk->setActive(x, y, 0, active);
    return;
  }

  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].active = active;
}
void Chunk::setFaces(int x, int y, int z, bool faces[]){
  if (x < 0){
    if (this->leftChunk->isLoaded) this->leftChunk->setFaces(WIDTH_CHUNK-1, y, z, faces);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk->isLoaded) this->rightChunk->setFaces(0, y, z, faces);
    return;
  }
  if (z < 0){
    if (this->bottomChunk->isLoaded) this->bottomChunk->setFaces(x, y, WIDTH_CHUNK-1, faces);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk->isLoaded) this->upChunk->setFaces(x, y, 0, faces);
    return;
  }

  for (int i = 0; i < 6; i++) 
      data[z * HEIGHT_CHUNK * WIDTH_CHUNK + y * WIDTH_CHUNK + x].faces[i] = faces[i];
}
void Chunk::setFace(int x, int y, int z, int face, bool active){
  if (x < 0){
    if (this->leftChunk->isLoaded) this->leftChunk->setFace(WIDTH_CHUNK-1, y, z, face, active);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk->isLoaded) this->rightChunk->setFace(0, y, z, face, active);
    return;
  }
  if (z < 0){
    if (this->bottomChunk->isLoaded) this->bottomChunk->setFace(x, y, WIDTH_CHUNK-1, face, active);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk->isLoaded) this->upChunk->setFace(x, y, 0, face, active);
    return;
  }
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].faces[face] = active;
}
void Chunk::setType(int x, int y, int z, BLOCK_TYPE type){

  if (x < 0){
    if (this->leftChunk != nullptr && this->leftChunk->isLoaded) this->leftChunk->setType(WIDTH_CHUNK-1, y, z, type);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk != nullptr && this->rightChunk->isLoaded) this->rightChunk->setType(0, y, z, type);
    return;
  }
  if (z < 0){
    if (this->bottomChunk != nullptr && this->bottomChunk->isLoaded) this->bottomChunk->setType(x, y, WIDTH_CHUNK-1, type);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk != nullptr && this->upChunk->isLoaded) this->upChunk->setType(x, y, 0, type);
    return;
  }

  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].type = type;
}

