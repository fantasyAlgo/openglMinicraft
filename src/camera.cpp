#include "hFiles/camera.h"
#include <cstddef>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

Camera::Camera(int WIDTH, int HEIGHT){
  this->position = glm::vec3(0.0f, 0.0f, 0.0f);
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

void Camera::inputHandling(GLFWwindow *window, float deltaTime){
  const float cameraSpeed = 3.0f * deltaTime; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    this->position += cameraSpeed * this->direction;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    this->position -= cameraSpeed * this->direction;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    this->position -= glm::normalize(glm::cross(this->direction, this->cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    this->position += glm::normalize(glm::cross(this->direction, this->cameraUp)) * cameraSpeed;

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		// Prevents camera from jumping on the first click
		if (firstClick){
			glfwSetCursorPos(window, (WIDTH/ 2), (HEIGHT/ 2));
			firstClick = false;
		}
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
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}
