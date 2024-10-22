#include "hFiles/camera.h"
#include "hFiles/chunk.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>


glm::vec3 facesPosition[] = {
  glm::vec3(0.0, 0.0, 0.5),
  glm::vec3(0.0, 0.0, -0.5),
  glm::vec3(0.5, 0.0, 0.0),
  glm::vec3(-0.5, 0.0, 0.0),
  glm::vec3(0.0, 0.5, 0.0),
  glm::vec3(0.0, -0.5, 0.0)
};


Camera::Camera(int WIDTH, int HEIGHT){
  this->position = glm::vec3(0.0f, 16, 0.0f);
  this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
  this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  this->WIDTH = WIDTH;
  this->HEIGHT = HEIGHT;
  this->pitch = 0;
  this->yaw = 0;
  this->lastX = WIDTH/2;
  this->lastY = HEIGHT/2;
}
glm::mat4 Camera::CameraLookAt(){
  return glm::lookAt(position, position + direction, this->cameraUp);
}

void Camera::updatePointer(Chunk &chunk){
  glm::vec3 cameraPointer = this->position;
  while (chunk.isInside(cameraPointer) && !chunk.data[(int)cameraPointer.x][(int)cameraPointer.y][(int)cameraPointer.z].active)
    cameraPointer += this->direction;
  glm::vec3 before = cameraPointer - this->direction;
  if (chunk.data[(int)cameraPointer.x][(int)cameraPointer.y][(int)cameraPointer.z].active){
    this->active_pointer_block = true;
    cameraPointer = glm::vec3((int)cameraPointer.x+0.5, (int)cameraPointer.y + 0.5, (int) cameraPointer.z + 0.5);
    Face face;
    float minV = 10000;
    int minI = 0;
    glm::vec3 current;
    for (int i = 0; i < 6; i++) {
      current = cameraPointer + facesPosition[i];
      if (!chunk.data[(int)current.x][(int)current.y][(int)current.z].active && glm::distance(before, cameraPointer + facesPosition[i]) < minV){
        minV = glm::distance(before, cameraPointer + facesPosition[i]);
        minI = i;
      }
    }
    cameraPointer = cameraPointer + facesPosition[minI];
    cameraPointer = glm::vec3((int)cameraPointer.x, (int)cameraPointer.y, (int)cameraPointer.z);
    this->pointer_block.first = cameraPointer;
    this->pointer_block.second = (Face)minI;
  }else this->active_pointer_block = false;
}

void Camera::mouseHandling(GLFWwindow* window, Chunk &chunk, float deltaTime){
  // Hides mouse cursor
  if (!firstClick && !rightPressed && this->active_pointer_block && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
    chunk.AddBlock(this->pointer_block.first);
    rightPressed = true;
  }
  if (!firstClick && !leftPressed && this->active_pointer_block && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
    std::cout << "Left pressed!" << std::endl;
    chunk.RemoveBlock(this->pointer_block.first - facesPosition[this->pointer_block.second]);
    leftPressed = true;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) rightPressed = false;
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) leftPressed = false;

	if (firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    firstClick = false;
    glfwSetCursorPos(window, (WIDTH/ 2), (HEIGHT/ 2));
  }

  if (firstClick) return;
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  float xOffset = xpos - lastX; lastX = xpos;
  float yOffset = ypos - lastY; lastY = ypos;
  const float sensitivity = 0.2;
  xOffset *= sensitivity; yOffset *= sensitivity;
  yaw += xOffset; pitch -= yOffset;
  //std::cout << yaw << " | " << pitch << " | " << xpos << " | " << ypos << std::endl;
  if (pitch > 89.0) pitch = 89.0f;
  if (pitch < -89.0) pitch = -89.0f;
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  this->direction = glm::normalize(front);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL)
}
void Camera::keyboardHandling(GLFWwindow *window, float deltaTime){
  const float cameraSpeed = 3.0f * deltaTime; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    this->position += cameraSpeed * this->direction;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    this->position -= cameraSpeed * this->direction;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    this->position -= glm::normalize(glm::cross(this->direction, this->cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    this->position += glm::normalize(glm::cross(this->direction, this->cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    this->position += cameraSpeed * this->cameraUp;
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    this->position -= cameraSpeed * this->cameraUp;
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    firstClick = true;
  }
}
