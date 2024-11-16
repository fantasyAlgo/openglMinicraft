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
#include <glm/geometric.hpp>
#include <iostream>

#include <cstdint>
#include <cassert>
#include <string>

#include <fstream>
#include <filesystem>
#include <cmath>
void Chunk::updateLight(char ambient_lights=15){
  char ambient_light = 15;
  int light = 0;
  int lastLight[WIDTH_CHUNK][WIDTH_CHUNK] = {0};
  for (int y = HEIGHT_CHUNK-1; y >= 0; y--) {
    for (int x = 0; x < WIDTH_CHUNK; x++) {
      for (int z = 0; z < WIDTH_CHUNK; z++) {
        //light = y > HEIGHT_CHUNK;
        if (getActive(x, y, z))
          lastLight[x][z] = (lastLight[x][z] == 0 ? y : lastLight[x][z]);
        light = std::max(1, 15-(lastLight[x][z]-y)/2);
        for (glm::vec3 light_pos : this->lights_pos)
          light = std::max(light, (int)(15-glm::distance(light_pos, glm::vec3(x,y,z))));
        if (this->leftChunk != nullptr){
          for (glm::vec3 light_pos : this->leftChunk->lights_pos)
            light = std::max(light, (int)(15-glm::distance(light_pos, glm::vec3(x,y,z))));
        }
        if (this->rightChunk != nullptr){
          for (glm::vec3 light_pos : this->rightChunk->lights_pos)
            light = std::max(light, (int)(15-glm::distance(light_pos, glm::vec3(x,y,z))));
        }
        if (this->upChunk != nullptr){
          for (glm::vec3 light_pos : this->upChunk->lights_pos)
            light = std::max(light, (int)(15-glm::distance(light_pos, glm::vec3(x,y,z))));
        }
        if (this->bottomChunk != nullptr){
          for (glm::vec3 light_pos : this->bottomChunk->lights_pos)
            light = std::max(light, (int)(15-glm::distance(light_pos, glm::vec3(x,y,z))));
        }
        this->setLightValue(x, y, z, light);
      }

    }
  }

}
