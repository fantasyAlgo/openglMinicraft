#include "hFiles/block.h"
#include <glm/ext/vector_float2.hpp>
#include <map>

glm::vec2 type_position[N_TYPES][3] = {
  {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 0.0f)},
  {glm::vec2(2.0f, 0.0f), glm::vec2(2.0f, 0.0f), glm::vec2(2.0f, 0.0f)},
  {glm::vec2(3.0f, 0.0f), glm::vec2(3.0f, 0.0f), glm::vec2(3.0f, 0.0f)},
  {glm::vec2(2.0f, 2.0f), glm::vec2(2.0f, 2.0f), glm::vec2(2.0f, 2.0f)},
  {glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
  {glm::vec2(2.0f, 1.0f), glm::vec2(3.0f, 1.0f), glm::vec2(3.0f, 1.0f)},
  {glm::vec2(6.0f, 3.0f), glm::vec2(6.0f, 3.0f), glm::vec2(6.0f, 3.0f)},
  {glm::vec2(0.0f, 15.0f), glm::vec2(0.0f, 15.0f), glm::vec2(0.0f, 15.0f)},
  {glm::vec2(3.0f, 2.0f), glm::vec2(3.0f, 2.0f), glm::vec2(3.0f, 2.0f)},
  {glm::vec2(6.0f, 1.0f), glm::vec2(6.0f, 1.0f), glm::vec2(6.0f, 1.0f)},
  {glm::vec2(0.0f, 3.0f), glm::vec2(0.0f, 3.0f), glm::vec2(0.0f, 3.0f)},
  {glm::vec2(2.0f, 3.0f), glm::vec2(0.0f, 3.0f), glm::vec2(0.0f, 3.0f)},
  {glm::vec2(5.0f, 5.0f), glm::vec2(5.0f, 5.0f), glm::vec2(5.0f, 5.0f)},
};

std::map<int, int> pos_to_type;
void initMapType(){
  for (int i = 0; i < N_TYPES; i++) {
    pos_to_type[type_position[i][0].x*16 + type_position[i][0].y] = i;
    pos_to_type[type_position[i][1].x*16 + type_position[i][1].y] = i;
    pos_to_type[type_position[i][2].x*16 + type_position[i][2].y] = i;
  }
}
bool isBillBoard[N_TYPES] = {
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 1,
 1,
 0,
};
