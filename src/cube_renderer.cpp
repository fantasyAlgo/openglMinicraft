#include "hFiles/cube_renderer.h"
#include <glm/ext/matrix_transform.hpp>

float getPosAtlasX(int x){
  return (1.0f/16.0f)*x;
}
float getPosAtlasY(int y){
  return (1.0f/16.0f)*y;
}

CubeRenderer::CubeRenderer(Shader &shader, Texture2D &texture){
  this->texture = texture;
  this->shader = shader;
  this->initRenderData();
}
void CubeRenderer::Render(glm::vec3 position, float rad){
  glActiveTexture(GL_TEXTURE0);
  this->texture.Bind();
  this->shader.Use();
  glBindVertexArray(this->cubeVAO);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(rad), glm::vec3(0.5f, 1.0f, 0.0f));
  shader.SetMatrix4("model", model);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}



void CubeRenderer::initRenderData(){
  float offset = 1.0f/16.0f;
  float blockX = getPosAtlasX(2) + offset;
  float blockY = 1-getPosAtlasY(2);
  float vertices[] = {
      // Positions          // Texture Coords
      // Front face
      -0.5f, -0.5f,  0.5f,  blockX-offset, blockY-offset,  // Bottom-left
       0.5f, -0.5f,  0.5f,  blockX, blockY-offset,  // Bottom-right
       0.5f,  0.5f,  0.5f,  blockX, blockY,  // Top-right
      -0.5f,  0.5f,  0.5f,  blockX-offset, blockY,  // Top-left
      // Back face
      -0.5f, -0.5f, -0.5f,  blockX-offset, blockY-offset,  // Bottom-left
       0.5f, -0.5f, -0.5f,  blockX, blockY-offset,  // Bottom-right
       0.5f,  0.5f, -0.5f,  blockX, blockY,  // Top-right
      -0.5f,  0.5f, -0.5f,  blockX-offset, blockY,  // Top-left
      // Left face
      -0.5f, -0.5f, -0.5f,  blockX-offset, blockY-offset,  // Bottom-left
      -0.5f, -0.5f,  0.5f,  blockX, blockY-offset,  // Bottom-right
      -0.5f,  0.5f,  0.5f,  blockX, blockY,  // Top-right
      -0.5f,  0.5f, -0.5f,  blockX-offset, blockY,  // Top-left
      // Right face
       0.5f, -0.5f, -0.5f,  blockX-offset, blockY-offset,  // Bottom-left
       0.5f, -0.5f,  0.5f,  blockX, blockY-offset,  // Bottom-right
       0.5f,  0.5f,  0.5f,  blockX, blockY,  // Top-right
       0.5f,  0.5f, -0.5f,  blockX-offset, blockY,  // Top-left
      // Top face
      -0.5f,  0.5f,  0.5f,  blockX-offset, blockY-offset,  // Bottom-left
       0.5f,  0.5f,  0.5f,  blockX, blockY-offset,  // Bottom-right
       0.5f,  0.5f, -0.5f,  blockX, blockY,  // Top-right
      -0.5f,  0.5f, -0.5f,  blockX-offset, blockY,  // Top-left
      // Bottom face
      -0.5f, -0.5f,  0.5f,  blockX-offset, blockY-offset,  // Bottom-left
       0.5f, -0.5f,  0.5f,  blockX, blockY-offset,  // Bottom-right
       0.5f, -0.5f, -0.5f,  blockX, blockY,  // Top-right
      -0.5f, -0.5f, -0.5f,  blockX-offset, blockY   // Top-left
  };
  unsigned int indices[] = {
      // Front face
      0, 1, 2,
      0, 2, 3,
      // Back face
      4, 5, 6,
      4, 6, 7,
      // Left face
      8, 9, 10,
      8, 10, 11,
      // Right face
      12, 13, 14,
      12, 14, 15,
      // Top face
      16, 17, 18,
      16, 18, 19,
      // Bottom face
      20, 21, 22,
      20, 22, 23
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



