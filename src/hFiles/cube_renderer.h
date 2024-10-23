#pragma once

#include <iostream>
#include "shader.h"
#include "texture.h"
#include "block.h"

#include <GLFW/glfw3.h>
#include<glad/glad.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

class CubeRenderer {
private:
  uint cubeVAO;
  Shader shader;
  Texture2D texture;

  glm::vec3 faceOffsets[6];
  float faceRadiuses[6];
  bool faceAxis[6];
  void initRenderData();
public:
  CubeRenderer(Shader &shader, Texture2D &texture);
  void Render(glm::vec3 position, Block block);
};
