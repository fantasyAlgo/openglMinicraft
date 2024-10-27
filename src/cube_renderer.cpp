#include "hFiles/cube_renderer.h"
#include "hFiles/block.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>

float getPosAtlasX(int x){
  return (1.0f/16.0f)*x;
}
float getPosAtlasY(int y){
  return (1.0f/16.0f)*y;
}

CubeRenderer::CubeRenderer(Shader &shader, Texture2D &texture){
  this->texture = texture;
  this->shader = shader;

  glm::vec3 faceOffsets[6] = {
    glm::vec3(0.0, 0.0, 0.5),
    glm::vec3(0.0, 0.0, -0.5),
    glm::vec3(-0.5, 0.0, 0.0),
    glm::vec3(0.5, 0.0, 0.0),
    glm::vec3(0.0, 0.5, 0.0),
    glm::vec3(0.0, -0.5, 0.0),
  };
  float faceRadiuses[6] = {0, 0, 90, 90, 90, 90};
  bool faceAxis[6] = {0, 0, 0, 0, 1, 1};
  for (int i = 0; i < 6; i++) {
    this->faceOffsets[i] = faceOffsets[i];
    this->faceRadiuses[i] = faceRadiuses[i];
    this->faceAxis[i] = faceAxis[i];
  }
  this->initRenderData();
}

void CubeRenderer::Render(glm::vec3 position, Block block){
  glActiveTexture(GL_TEXTURE0);
  this->texture.Bind();
  this->shader.Use();
  glBindVertexArray(this->cubeVAO);
  if (block.type > END_BLOCK) return;
  //std::cout << position.x << " | " << position.y << "| " << position.z << std::endl;
  shader.SetVector2f("texture_pos", type_position[(int)block.type][0]);
  for (int i = 0; i < 6; i++) {
    if (!block.faces[i]) continue;
    if (i >= 4)
      shader.SetVector2f("texture_pos", type_position[(int)block.type][i-3]);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position + this->faceOffsets[i]);
    model = glm::rotate(model, glm::radians(this->faceRadiuses[i]), glm::vec3((float)(this->faceAxis[i]), (float)(!this->faceAxis[i]), 0.0f));
    shader.SetMatrix4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}


void CubeRenderer::initRenderData(){
  //float offset = 1.0f/16.0f;
  //float blockX = getPosAtlasX(2) + offset;
  //float blockY = 1-getPosAtlasY(2);
  float vertices[] = {
      // Positions          // Texture Coords
       0.5f,  0.5f, 0.0f,   1, 1,   // Top right
       0.5f, -0.5f, 0.0f,   1, 0,   // Bottom right
      -0.5f, -0.5f, 0.0f,   0, 0,   // Bottom left
      -0.5f,  0.5f, 0.0f,   0, 1// Top left
  };

  unsigned int indices[] = {
      0, 1, 3,   // First triangle (top-right)
      1, 2, 3    // Second triangle (bottom-left)
  };

  uint VBO, EBO;
  glGenVertexArrays(1, &this->cubeVAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);


  glBindVertexArray(this->cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

}



