#pragma once

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

class Camera {
private:
  bool firstClick;
  float lastX, lastY;
  int HEIGHT, WIDTH;
  float yaw, pitch;
public:
  glm::vec3 cameraUp;
  glm::vec3 position;
  glm::vec3 direction;
  Camera(int WIDTH, int HEIGHT);
  //void mouse_callback(GLFWwindow* window, double xPos, double yPos);
  glm::mat4 CameraLookAt();
  void inputHandling(GLFWwindow *window, float deltaTime);
};
