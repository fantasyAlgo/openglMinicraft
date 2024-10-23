#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 texture_pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main(){
  vec4 end_f = proj * view * model * vec4(aPos, 1.0);
  gl_Position = end_f;//vec4(aPos, 1.0f);

  float offset = 1.0f/16.0f;
  float blockX = offset*texture_pos.x + offset;
  float blockY = 1-offset*texture_pos.y;
  TexCoord = vec2(aTexCoord.x*blockX + (1-aTexCoord.x)*(blockX-offset),
                  aTexCoord.y*blockY + (1-aTexCoord.y)*(blockY-offset));
}
