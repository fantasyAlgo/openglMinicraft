#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;


// vec3(x_tex, y_tex, face)
uniform vec3 texture_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

const vec3 normals[6] = vec3[6](
    vec3(0.0, 0.0, 1.0),  // FRONT: Normal along +Z axis
    vec3(0.0, 0.0, -1.0), // BACK: Normal along -Z axis
    vec3(-1.0, 0.0, 0.0), // LEFT: Normal along -X axis
    vec3(1.0, 0.0, 0.0),  // RIGHT: Normal along +X axis
    vec3(0.0, 1.0, 0.0),  // UP: Normal along +Y axis
    vec3(0.0, -1.0, 0.0)  // BOTTOM: Normal along -Y axis
);

void main(){
  vec4 end_f = proj * view * model * vec4(aPos, 1.0);
  gl_Position = end_f;//vec4(aPos, 1.0f);

  float offset = 1.0f/16.0f;
  float blockX = offset*texture_pos.x + offset;
  float blockY = 1-offset*texture_pos.y;
  TexCoord = vec2(aTexCoord.x*blockX + (1-aTexCoord.x)*(blockX-offset),
                  aTexCoord.y*blockY + (1-aTexCoord.y)*(blockY-offset));
  Normal = normals[int(texture_pos.z)];
  FragPos = vec3(model * vec4(aPos, 1.0));
}
