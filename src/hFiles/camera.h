#pragma once

#include "block.h"
#include "chunk.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <utility>

enum Face {
  FRONT,
  BACK,
  LEFT,
  RIGHT,
  UP,
  BOTTOM,
  NO_FACE
};
extern glm::vec3 facesPosition[];

struct BlockPointer {
  glm::vec3 pos;
  Face face;
  glm::vec2 chunk_pos;
};


class Camera {
private:
  bool firstClick, leftPressed, rightPressed, sprint = false;
  float lastX, lastY;
  int HEIGHT, WIDTH;
  float yaw, pitch;
public:
  BLOCK_TYPE place_type = GRASS;
  glm::vec3 cameraUp;
  glm::vec3 position;
  glm::vec3 direction;

  bool active_pointer_block;
  BlockPointer pointer_block;

  Camera(int WIDTH, int HEIGHT);

  
  // updates active_pointer_block if a block was found, and finds the nearest pointer_block
  void updatePointer(Chunk &chunk);
  // Returns the LookAt matrix using the position and direction of the camera
  glm::mat4 CameraLookAt();
  // Handles input and mouse events
  void keyboardHandling(GLFWwindow *window, float deltaTime);
  void mouseHandling(GLFWwindow* window, Chunk &chunk, float deltaTime);
};
