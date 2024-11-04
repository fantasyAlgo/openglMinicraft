#include "hFiles/chunk.h"
#include "hFiles/Settings.h"
#include "hFiles/block.h"
#include <algorithm>
#include <any>
#include <climits>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
//#include "hFiles/Settings.h"

#include <cstdint>
#include <cassert>
#include <iostream>

#include <cstdint>
#include <cassert>

uint32_t packInstanceData(uint8_t x, uint8_t y, uint8_t z, uint8_t face, uint8_t texture, bool isBillBoard = false) {
    // Check each parameter to ensure it fits within the specified bit limits
    assert(x <= 0x3F && "x must fit in 6 bits (0-63)");
    assert(y <= 0x3F && "y must fit in 6 bits (0-63)");
    assert(z <= 0x3F && "z must fit in 6 bits (0-63)");
    assert(face >= 0 && face <= 5 && "face must fit in 4 bits (0-15)");
    assert(texture <= 0x7F && "texture must fit in 7 bits (0-127)");

    // Pack the data
    uint32_t packedData = 0;
    packedData |= (x & 0x3F) << 0;        // 6 bits for x
    packedData |= (y & 0x3F) << 6;        // 6 bits for y
    packedData |= (z & 0x3F) << 12;       // 6 bits for z
    packedData |= (face & 0x0F) << 18;    // 4 bits for face
    packedData |= (texture & 0x7F) << 22; // 7 bits for texture
    packedData |= (isBillBoard ? 1 : 0) << 29; // 1 bit for isBillBoard

    return packedData;
}
bool isTransparent(Block block){
  return block.type == LEAVES || block.type == FLOWER || block.type == GRASS_FLOWER;
}

void Chunk::update(){ this->updateFaces(); this->updatePackedData();}


Chunk::Chunk(): active(false), isLoaded(false), offset(glm::vec2(0.0f, 0.0f)){
  data.resize(WIDTH_CHUNK*WIDTH_CHUNK*HEIGHT_CHUNK);
}
void Chunk::Render(CubeRenderer &cubeRenderer){
  cubeRenderer.RenderChunk(this->offset, this->n_packed_data, this->packed_data);
}
void Chunk::RenderWater(CubeRenderer &cubeRenderer){
  cubeRenderer.RenderChunk(this->offset, this->n_packed_data_water, this->packed_data_water);
}

void Chunk::AddBlock(glm::vec3 position, BLOCK_TYPE type){
  int x = position.x; int y = position.y; int z = position.z;
  //std::cout << "pos: " << x << " | " << y << " | " << z << std::endl;
  this->setActive(x, y, z, true);
  this->setType(x, y, z, type);
  this->updateFace(x, y, z);

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
  //std::cout << "chunks: " << this->leftChunk->isLoaded << " " << this->rightChunk->isLoaded << " " << this->bottomChunk->isLoaded << " " << this->upChunk->isLoaded << std::endl;
  //std::cout << "block z+1: " << this->get(x, y, z+1).faces[0] << this->get(x, y, z+1).faces[1] << this->get(x, y, z+1).faces[2]  << this->get(x, y, z+1).faces[3] << this->get(x, y, z+1).faces[4] << this->get(x, y, z+1).faces[5] << std::endl;
  this->setActive(x, y, z, false);
  this->setType(x, y, z, END_BLOCK);
  this->setFace(x-1, y, z, 3, true);
  this->setFace(x+1, y, z, 2, true);
  this->setFace(x, y-1, z, 4, true);
  this->setFace(x, y+1, z, 5, true);
  this->setFace(x, y, z-1, 0, true);
  this->setFace(x, y, z+1, 1, true);
  //std::cout << "block z+1: " << this->get(x, y, z+1).faces[0] << this->get(x, y, z+1).faces[1] << this->get(x, y, z+1).faces[2]  << this->get(x, y, z+1).faces[3] << this->get(x, y, z+1).faces[4] << this->get(x, y, z+1).faces[5] << std::endl;

}


void Chunk::InitChunk(const siv::PerlinNoise &perlin, glm::vec2 chunk_pos, Chunk *up, Chunk *down, Chunk *left, Chunk *right){
  this->upChunk = up;
  this->bottomChunk = down;
  this->leftChunk = left;
  this->rightChunk = right;

  this->map_pos = chunk_pos;
  this->offset = (float)WIDTH_CHUNK*chunk_pos;
  this->MakeChunkData(perlin);

  this->update();
  if (up != nullptr)
    up->update();
  if (down != nullptr)
    down->update();
  if (left != nullptr)
    left->update();
  if (right != nullptr)
    right->update();

  this->isLoaded = true;
}


void Chunk::MakeTrees(int x, int startY, int z){
  int howTall = rand()%6+ 4;
  int width = 2;
  int heightLeaves = 3;
  for (int k = 0; k < heightLeaves; k++) {
    for (int i = -width+k; i <= width-k; i++) {
      for (int j = -width+k; j <= width-k; j++) {
        if (abs(i) == width && abs(j) == width) continue;
        this->setType(x+i, startY+howTall+k, z+j, LEAVES);
        this->setActive(x+i, startY+howTall+k, z+j, true);
        }
      }
  }
  for (int y = startY; y < (int)startY+howTall+1; y++) {
    this->setType(x, y, z, TREE_BLOCK);
    this->setActive(x, y, z, true);
  }

}

void Chunk::MakeChunkData(const siv::PerlinNoise &perlin){
  float maxY, cavePerlin, treeY;
  cavePerlin = 1.0f;
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int z = 0; z < WIDTH_CHUNK; z++) {
      maxY = (int) (HEIGHT_CHUNK*perlin.octave2D_01(((float)(x+offset.x) * 0.025), ((float)(z+offset.y) * 0.025), 4));
      if (maxY >= WATER_LEVEL+SAND_LEVEL && maxY < MOUNTAINS_HEIGHT && rand()/100000000.0f < 0.4)
        this->MakeTrees(x, maxY, z);
      else if (maxY > WATER_LEVEL+SAND_LEVEL && maxY < MOUNTAINS_HEIGHT && (rand()%10000)/10000.0 < 0.1){
        if (rand()%2 == 0)
          this->setType(x, maxY+1, z, GRASS_FLOWER);
        else this->setType(x, maxY+1, z, FLOWER);
        this->setActive(x, maxY+1, z, true);
      }
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
            }else if (maxY < WATER_LEVEL){
              this->setType(x, y, z, SAND);
            }else {
              if (y > maxY-4) this->setType(x, y, z, DIRT);
              if (y == maxY) this->setType(x, y, z, GRASS);
            }
            this->setActive(x, y, z, true);
          }
        }
        if (!this->get(x, y, z).active){
          this->setType(x, y, z, END_BLOCK);
        }
      }
    }
  }
  this->updateFaces();
}

void Chunk::updatePackedData(){
  int size = data.size();
  this->n_packed_data = 0;
  int indx = 0;
  int indx_water = 0;
  glm::vec2 text;
  Block curr;
  bool foundTree = false;
  int max_faces;
  for (int x = 0; x < WIDTH_CHUNK; x++) {
    for (int y = 0; y < HEIGHT_CHUNK; y++) {
      for (int z = 0; z < WIDTH_CHUNK; z++) {
        curr = get(x,y,z);
        if (!curr.active) continue;
        max_faces = isBillBoard[(int)curr.type] ? 2 : 6;
        for (int f = 0; f < max_faces; f++) {
          if (!curr.faces[f]) continue;
          text = type_position[(int)curr.type][f < 4 ? 0 : f-3];
          if (curr.type != WATER_BASIC)
            packed_data[indx++] = packInstanceData(x, y, z, f, text.x*16 + text.y, isBillBoard[(int)curr.type]);
          else
            packed_data_water[indx_water++] = packInstanceData(x, y, z, f, text.x*16 + text.y, isBillBoard[(int)curr.type]);
        }
      }
    }
  }
  this->n_packed_data = indx;
  this->n_packed_data_water = indx_water;
}

bool isFaceVisible(Block originalBlock, Block block){
  if (isTransparent(block)) return true;
  if (originalBlock.type == WATER_BASIC) return block.type == END_BLOCK;
  else return block.type == WATER_BASIC || !block.active;
}
void Chunk::updateFace(int x, int y, int z){
  Block curr = get(x,y,z);
  bool new_faces[] = {
    isFaceVisible(curr, get(x,y,z+1)),
    isFaceVisible(curr, get(x,y,z-1)),
    isFaceVisible(curr, get(x-1,y,z)),
    isFaceVisible(curr, get(x+1,y,z)),
    isFaceVisible(curr, get(x,y+1,z)),
    isFaceVisible(curr, get(x,y-1,z))
  };
  this->setFaces(x, y, z, new_faces);
}
void Chunk::updateFaces(){
  for (int x = 0; x < WIDTH_CHUNK; x++)
    for (int z = 0; z < WIDTH_CHUNK; z++) 
      for (int y = 0; y < HEIGHT_CHUNK; y++){
        if (this->leftChunk != nullptr && x == 0) this->leftChunk->updateFace(WIDTH_CHUNK-1, y, z);
        if (this->rightChunk != nullptr && x == WIDTH_CHUNK-1) this->rightChunk->updateFace(0, y, z);
        if (this->bottomChunk != nullptr && z == 0) this->bottomChunk->updateFace(x, y, WIDTH_CHUNK-1);
        if (this->upChunk != nullptr && z == WIDTH_CHUNK-1) this->upChunk->updateFace(x, y, 0);
        this->updateFace(x, y, z);
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
    if (this->leftChunk != nullptr && this->leftChunk->isLoaded) this->leftChunk->setActive(WIDTH_CHUNK-1, y, z, active);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk != nullptr && this->rightChunk->isLoaded) this->rightChunk->setActive(0, y, z, active);
    return;
  }
  if (z < 0){
    if (this->bottomChunk != nullptr && this->bottomChunk->isLoaded) this->bottomChunk->setActive(x, y, WIDTH_CHUNK-1, active);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk  != nullptr && this->upChunk->isLoaded) this->upChunk->setActive(x, y, 0, active);
    return;
  }
  this->needsUpdate = true;
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].active = active;
}
void Chunk::setFaces(int x, int y, int z, bool faces[]){
  if (x < 0){
    if (this->leftChunk != nullptr && this->leftChunk->isLoaded) this->leftChunk->setFaces(WIDTH_CHUNK-1, y, z, faces);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk != nullptr && this->rightChunk->isLoaded) this->rightChunk->setFaces(0, y, z, faces);
    return;
  }
  if (z < 0){
    if (this->bottomChunk != nullptr && this->bottomChunk->isLoaded) this->bottomChunk->setFaces(x, y, WIDTH_CHUNK-1, faces);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk != nullptr && this->upChunk->isLoaded) this->upChunk->setFaces(x, y, 0, faces);
    return;
  }
  this->needsUpdate = true;
  for (int i = 0; i < 6; i++) 
      data[z * HEIGHT_CHUNK * WIDTH_CHUNK + y * WIDTH_CHUNK + x].faces[i] = faces[i];
}
void Chunk::setFace(int x, int y, int z, int face, bool active){
  if (x < 0){
    if (this->leftChunk != nullptr && this->leftChunk->isLoaded) this->leftChunk->setFace(WIDTH_CHUNK-1, y, z, face, active);
    return;
  }
  if (x >= WIDTH_CHUNK){
    if (this->rightChunk != nullptr && this->rightChunk->isLoaded) this->rightChunk->setFace(0, y, z, face, active);
    return;
  }
  if (z < 0){
    if (this->bottomChunk != nullptr && this->bottomChunk->isLoaded) this->bottomChunk->setFace(x, y, WIDTH_CHUNK-1, face, active);
    return;
  }
  if (z >= WIDTH_CHUNK){
    if (this->upChunk != nullptr && this->upChunk->isLoaded) this->upChunk->setFace(x, y, 0, face, active);
    return;
  }
  this->needsUpdate = true;
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
  this->needsUpdate = true;
  data[z*HEIGHT_CHUNK*WIDTH_CHUNK + y*WIDTH_CHUNK + x].type = type;
}

