#include "hFiles/Player.h"

#include "hFiles/Settings.h"
#include "hFiles/block.h"
#include "imgui.h"
#include <glm/ext/vector_float2.hpp>

Player::Player(){
  this->camera.InitCamera(WIDTH, HEIGHT);
  this->picked_item = 0;
  for (int x = 0; x < INVENTORY_COLUMNS; x++) {
    for (int y = 0; y < INVENTORY_ROWS; y++) {
      if (x == 0){
        this->inventory[x][y].amount = 2;
        this->inventory[x][y].type = (BLOCK_TYPE)y;
      }
    }
  }
}

struct UVCoords {
    ImVec2 uv0; // Top-left UV coordinate
    ImVec2 uv1; // Bottom-right UV coordinate
};
UVCoords GetUVForBlockType(BLOCK_TYPE type) {
  glm::vec2 texture_pos = type_position[(int)type][0];
  float offset = 1.0f/16.0f;
  float blockX = offset*texture_pos.x + offset;
  float blockY = 1-offset*texture_pos.y;
  return {{blockX, blockY}, {blockX-offset, blockY-offset}};
}
void Player::RenderInventory(GLuint atlasTexture) {
    ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
    glm::vec2 slotSize = glm::vec2(50, 50);
    glm::vec2 imageSize = glm::vec2(40, 40);


    for (int i = 0; i < INVENTORY_ROWS; i++) {
        ImVec2 slotPos = ImGui::GetCursorScreenPos();

        // Check if this is the selected slot and apply a different style
        ImU32 slotColor = (i == picked_item) ? IM_COL32(77, 128, 230, 255) : IM_COL32(50, 50, 50, 255);

        // Draw background for the slot
        ImGui::GetWindowDrawList()->AddRectFilled(slotPos, ImVec2(slotPos.x + slotSize.x, slotPos.y + slotSize.y), slotColor);
        ImGui::SameLine();
        if (inventory[0][i].type < END_BLOCK) {
            // Get UV coordinates for the current block type
            UVCoords uv = GetUVForBlockType(inventory[0][i].type);
            ImVec2 imagePos = ImVec2(
                slotPos.x + (slotSize.x - imageSize.x) * 0.5f,
                slotPos.y + (slotSize.y - imageSize.y) * 0.5f
            );
            // Draw the item image using the UV coordinates for the atlas
            ImGui::SetCursorScreenPos(imagePos);
            ImGui::Image(atlasTexture, ImVec2(imageSize.x, imageSize.y), uv.uv0, uv.uv1);
        }

        // Draw item count in the center of the slot if there are items
        if (inventory[0][i].amount > 0) {
            std::string countText = std::to_string(inventory[0][i].amount);
            ImVec2 textSize = ImGui::CalcTextSize(countText.c_str());
            ImVec2 textPos = ImVec2(
                slotPos.x + (slotSize.x - textSize.x) * 0.5f,
                slotPos.y + (slotSize.y - textSize.y) * 0.5f
            );
            ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), countText.c_str());
        }

        if (i < INVENTORY_ROWS - 1) {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void Player::setPosition(glm::vec3 position){
  this->camera.position = position;
}
void Player::setPickedItem(int i){
  this->picked_item = i;
  this->camera.place_type = (BLOCK_TYPE)i;
}

