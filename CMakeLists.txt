cmake_minimum_required(VERSION 3.10)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(learn_opengl
  src/Main.cpp 
  src/resource_manager.cpp
  src/shader.cpp
  src/texture.cpp
  src/cube_renderer.cpp
  src/camera.cpp
  src/chunk.cpp
  src/block.cpp
  src/glad.c)


find_package(glm CONFIG REQUIRED)
target_link_libraries(learn_opengl glfw GL dl glm::glm)

