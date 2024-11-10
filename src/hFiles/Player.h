#pragma once
#include "Settings.h"
#include "block.h"
#include "camera.h"
#include <glm/ext/vector_float3.hpp>

struct HAND_BLOCK{
  int amount;
  BLOCK_TYPE type;
};

class Player {
//private:
public:
  Camera camera;
  int picked_item;
  HAND_BLOCK inventory[INVENTORY_COLUMNS][INVENTORY_ROWS];
  Player();
  void RenderInventory(int WIDTH, int HEIGHT, GLuint atlasTexture);

  // Get|set stuff America needs!
  void setPosition(glm::vec3 position);
  void setPickedItem(int item);
};
