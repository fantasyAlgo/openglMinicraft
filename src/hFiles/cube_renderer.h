#pragma once

#include "shader.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include<glad/glad.h>
#include <glm/ext/vector_float2.hpp>
class CubeRenderer {
private:
  uint cubeVAO;
  Shader shader;
  Texture2D texture;
  void initRenderData();
public:
  CubeRenderer(Shader &shader, Texture2D &texture);
  void Render(glm::vec3 position, float rad);
};
