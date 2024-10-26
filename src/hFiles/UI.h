#pragma once

#include "imgui.h"

namespace UI {
  void DrawCross(ImVec2 center, float size, ImU32 color, float thickness){
      ImDrawList* draw_list = ImGui::GetForegroundDrawList(); // Use foreground draw list for global overlay

      // Calculate endpoints for horizontal and vertical lines
      ImVec2 horizontal_start(center.x - size / 2, center.y);
      ImVec2 horizontal_end(center.x + size / 2, center.y);
      ImVec2 vertical_start(center.x, center.y - size / 2);
      ImVec2 vertical_end(center.x, center.y + size / 2);

      // Draw the cross using intersecting lines
      draw_list->AddLine(horizontal_start, horizontal_end, color, thickness);
      draw_list->AddLine(vertical_start, vertical_end, color, thickness);
  }

  void RenderUI(int WIDTH, int HEIGHT){
    ImVec2 center(WIDTH/2, HEIGHT/2); 
    float size = 20.0f;
    ImU32 color = IM_COL32(255, 255, 255, 255); // Red color for the cross
    DrawCross(center, size, color, 3.0f);
  }
}
