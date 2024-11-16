cmake_minimum_required(VERSION 3.10)
project(Minicraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

set(IMGUI_DIR "imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw GL)


include_directories(include)
add_executable(Minicraft
  src/glad.c

  src/Main.cpp 
  src/Game.cpp
  src/resource_manager.cpp
  src/shader.cpp
  src/texture.cpp
  src/cube_renderer.cpp
  src/camera.cpp
  src/Player.cpp
  src/chunk.cpp
  src/block.cpp
  src/light.cpp
)


find_package(glm CONFIG REQUIRED)
target_link_libraries(Minicraft imgui glfw GL dl glm::glm)

