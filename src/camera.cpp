#include "hFiles/camera.h"
#include "hFiles/block.h"
#include "hFiles/chunk.h"
#include <GLFW/glfw3.h>
#include <climits>
#include <cmath>
#include <complex>
#include <cstddef>
#include <filesystem>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float2.hpp>
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
  glm::vec3(0.0, -0.5, 0.0),
  glm::vec3(0.0f, 0.0f, 0.0f)
};


Camera::Camera(int WIDTH, int HEIGHT){
  this->position = glm::vec3(0.0f, 16, 0.0f);
  this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
  this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  this->WIDTH = WIDTH;
  this->HEIGHT = HEIGHT;
  this->pitch = 0;
  this->yaw = 0;
  this->lastX = WIDTH/2.0;
  this->lastY = HEIGHT/2.0;
}
Camera::Camera(){
  this->position = glm::vec3(0.0f, 16, 0.0f);
  this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
  this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  this->pitch = 0;
  this->yaw = 0;
}
void Camera::InitCamera(int WIDTH, int HEIGHT){
  this->WIDTH = WIDTH;
  this->HEIGHT = HEIGHT;
  this->lastX = WIDTH/2.0;
  this->lastY = HEIGHT/2.0;
}

glm::mat4 Camera::CameraLookAt(){
  return glm::lookAt(position, position + direction, this->cameraUp);
}

void Camera::updatePointer(Chunk &initialChunk){
  //std::cout << "####################################" << std::endl;
  Chunk* chunk = &initialChunk;
  glm::vec3 cameraPointer = this->position - glm::vec3(chunk->offset.x, 0, chunk->offset.y);
  this->direction = glm::normalize(this->direction);

  //std::cout << "camera: " << cameraPointer.x << " " << cameraPointer.y << " " << cameraPointer.z << std::endl;
  //std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
  glm::vec3 unitStepSize = glm::vec3(sqrt(1 + (direction.y/direction.x)*(direction.y/direction.x) + (direction.z/direction.x)*(direction.z/direction.x) ),
                                     sqrt(1 + (direction.x/direction.y)*(direction.x/direction.y) + (direction.z/direction.y)*(direction.z/direction.y) ),
                                     sqrt(1 + (direction.x/direction.z)*(direction.x/direction.z) + (direction.y/direction.z)*(direction.y/direction.z) ));
  glm::vec3 vMapCheck = glm::vec3((int)cameraPointer.x, (int)cameraPointer.y, (int)cameraPointer.z);
  //std::cout << "pos: " << vMapCheck.x << " " << vMapCheck.y << " " << vMapCheck.z << std::endl;
  glm::vec3 vRayLength1D;
  glm::vec3 vStep;

  if (direction.x < 0){vStep.x = -1; vRayLength1D.x = (cameraPointer.x - float(vMapCheck.x)) * unitStepSize.x;}
  else {vStep.x = 1; vRayLength1D.x = (float(vMapCheck.x+1)-cameraPointer.x) * unitStepSize.x;}

  if (direction.y < 0){vStep.y = -1; vRayLength1D.y = (cameraPointer.y - float(vMapCheck.y)) * unitStepSize.y;}
  else {vStep.y = 1; vRayLength1D.y = (float(vMapCheck.y+1)-cameraPointer.y) * unitStepSize.y;}

  if (direction.z < 0){vStep.z = -1; vRayLength1D.z = (cameraPointer.z - float(vMapCheck.z)) * unitStepSize.z;}
  else {vStep.z = 1; vRayLength1D.z = (float(vMapCheck.z+1)-cameraPointer.z) * unitStepSize.z;}

  //std::cout << "vRay: " << vRayLength1D.x << " " << vRayLength1D.y << " " << vRayLength1D.z << std::endl;

  bool bTileFound = false;
  float fMaxDistance = 100.0f;
  float fDistance = 0.0f;
  int times = 0;
  int lastAxisCollided;
  //std::cout << "Passages: " << std::endl;
  while (!bTileFound && fDistance < fMaxDistance && times < 1000){
    //std::cout << vMapCheck.x << " " << vMapCheck.y << " " << vMapCheck.z << std::endl;
    if (!chunk->isLoaded) break;
    times++;
    if (vRayLength1D.x < std::min(vRayLength1D.y, vRayLength1D.z)){
      vMapCheck.x += vStep.x;
      fDistance = vRayLength1D.x;
      vRayLength1D.x += unitStepSize.x;
      lastAxisCollided = 0;
    }
    else if (vRayLength1D.y < std::min(vRayLength1D.x, vRayLength1D.z)){
      vMapCheck.y += vStep.y;
      fDistance = vRayLength1D.y;
      vRayLength1D.y += unitStepSize.y;
      lastAxisCollided = 1;
    }
    else {
      vMapCheck.z += vStep.z;
      fDistance = vRayLength1D.z;
      vRayLength1D.z += unitStepSize.z;
      lastAxisCollided = 2;
    }

    if (vMapCheck.x < 0){
      vMapCheck.x = WIDTH_CHUNK-1;
      if (chunk->leftChunk != nullptr && chunk->leftChunk->isLoaded) chunk = chunk->leftChunk;
      else break;
    }
    if (vMapCheck.x >= WIDTH_CHUNK){
      vMapCheck.x = 0;
      //std::cout << chunk->rightChunk
      if (chunk->rightChunk != nullptr && chunk->rightChunk->isLoaded) chunk = chunk->rightChunk;
      else break;
    }
    if (vMapCheck.z < 0){
      vMapCheck.z = WIDTH_CHUNK-1;
      if (chunk->bottomChunk != nullptr && chunk->bottomChunk->isLoaded) chunk = chunk->bottomChunk;
      else break;
    }

    if (vMapCheck.z >= WIDTH_CHUNK){
      vMapCheck.z = 0;
      if (chunk->upChunk != nullptr && chunk->upChunk->isLoaded) chunk = chunk->upChunk;
      else break;
    }

    if (chunk->isInside(vMapCheck) && chunk->getActive(vMapCheck.x, vMapCheck.y, vMapCheck.z)){
      bTileFound = true;
    }
  }
  
  if (!bTileFound) return;
  glm::vec3 intrx = cameraPointer + direction*fDistance;
  if (lastAxisCollided == 0) this->pointer_block.face = (Face) (vStep.x == 1 ? 3 : 2);
  if (lastAxisCollided == 1) this->pointer_block.face = (Face) (vStep.y == 1 ? 5 : 4);
  if (lastAxisCollided == 2) this->pointer_block.face = (Face) (vStep.z == 1 ? 1 : 0);
  this->pointer_block.chunk_pos = chunk->map_pos;
  if (sprint){
    //std::cout << "vMa: " << vMapCheck.x << " " << vMapCheck.y << " " << vMapCheck.z << std::endl;
    //std::cout << "vS: " << vStep.x << " " << vStep.y << " " << vStep.z << std::endl;
    //std::cout << "faces: " << this->pointer_block.face << std::endl;
  }
  this->pointer_block.pos = vMapCheck;
  this->active_pointer_block = true;
}

void Camera::mouseHandling(GLFWwindow* window, Chunk &chunk, float deltaTime){
  // Hides mouse cursor
  if (!firstClick && !rightPressed && this->active_pointer_block && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
    if (this->pointer_block.pos.x == 0 && this->pointer_block.face == RIGHT){
      this->pointer_block.pos.x = WIDTH_CHUNK-1;
      this->pointer_block.face = NO_FACE;
      chunk.leftChunk->AddBlock(this->pointer_block.pos, place_type);
      chunk.leftChunk->needsUpdate = true;
    }
    else if (this->pointer_block.pos.x == WIDTH_CHUNK-1 && this->pointer_block.face == LEFT){
      this->pointer_block.pos.x = 0;
      this->pointer_block.face = NO_FACE;
      chunk.rightChunk->AddBlock(this->pointer_block.pos, place_type);
      chunk.rightChunk->needsUpdate = true;
    }
    else if (this->pointer_block.pos.z == 0 && this->pointer_block.face == BACK){
      this->pointer_block.pos.z = WIDTH_CHUNK-1;
      this->pointer_block.face = NO_FACE;
      chunk.bottomChunk->AddBlock(this->pointer_block.pos, place_type);
      chunk.bottomChunk->needsUpdate = true;
    }
    else if (this->pointer_block.pos.z == WIDTH_CHUNK-1 && this->pointer_block.face == FRONT){
      this->pointer_block.pos.z = 0;
      this->pointer_block.face = NO_FACE;
      chunk.upChunk->AddBlock(this->pointer_block.pos, place_type);
      chunk.upChunk->needsUpdate = true;
    }else{
      chunk.AddBlock(this->pointer_block.pos + 2.0f*facesPosition[(int)this->pointer_block.face], place_type);
      chunk.needsUpdate = true;
    }
    rightPressed = true;
  }
  if (!firstClick && !leftPressed && this->active_pointer_block && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
    //std::cout << "Left pressed!" << std::endl;
    //std::cout << "pos: " << this->pointer_block.pos.x << " " << this->pointer_block.pos.y << " " << this->pointer_block.pos.z << std::endl;
    chunk.RemoveBlock(this->pointer_block.pos);
    chunk.needsUpdate = true;
    if (chunk.leftChunk != nullptr) chunk.leftChunk->needsUpdate = true;
    if (chunk.rightChunk != nullptr) chunk.rightChunk->needsUpdate = true;
    if (chunk.bottomChunk != nullptr) chunk.bottomChunk->needsUpdate = true;
    if (chunk.upChunk != nullptr) chunk.upChunk->needsUpdate = true;

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
  const float cameraSpeed = (sprint ? 6.0f : 3.0f) * deltaTime; // adjust accordingly
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
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    this->sprint = true;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    this->sprint = false;

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    firstClick = true;
  }
}
