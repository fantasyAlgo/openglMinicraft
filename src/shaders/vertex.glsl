#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main(){
  vec4 end_f = proj * view * model * vec4(aPos, 1.0);
  gl_Position = end_f;//vec4(aPos, 1.0f);
  TexCoord = aTexCoord;
}
