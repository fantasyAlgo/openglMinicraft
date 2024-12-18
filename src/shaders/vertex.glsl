#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in int chunk_data;

// vec3(x_tex, y_tex, face)
uniform int data;
uniform vec2 chunk_offset;
//uniform vec3 texture_pos;
//uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float Visibility;
flat out int lightValue;

const vec3 normals[6] = vec3[6](
    vec3(0.0, 0.0, 1.0),  // FRONT: Normal along +Z axis
    vec3(0.0, 0.0, -1.0), // BACK: Normal along -Z axis
    vec3(-1.0, 0.0, 0.0), // LEFT: Normal along -X axis
    vec3(1.0, 0.0, 0.0),  // RIGHT: Normal along +X axis
    vec3(0.0, 1.0, 0.0),  // UP: Normal along +Y axis
    vec3(0.0, -1.0, 0.0)  // BOTTOM: Normal along -Y axis
);

vec3 packedPos;
int face; vec2 texture_pos; bool isBillBoard; int light_value;
void unpack(int packedData){
  packedPos.x = (packedData >> 0) & 0x1F;
  packedPos.y = (packedData >> 5) & 0x3F;
  packedPos.z = (packedData >> 11) & 0x1F;
  face = (packedData >> 16) & 0x07;
  int texPos = (packedData >> 19) & 0x7F;
  isBillBoard = bool((packedData >> 26) & 0x01);

  texture_pos.x = int(texPos/16);
  texture_pos.y = texPos%16;

  light_value = (packedData >> 27) & 15;
}
float gradient = 3.0;
float density = 0.009;
void main(){
  unpack(data);
  vec3 pos = aPos;
  if (isBillBoard){
    if (face == 0)
      pos.z = pos.x;
    else pos.z = 1.0f-pos.x;
    pos.z += (face == 0 ? 0.5 : -0.5);
  }else{
    if (face == 0)
      pos.z += 1.0;
    else if (face == 2){
      pos.xyz = pos.zyx;
      pos.xz += vec2(-0.5, 0.5);
    }else if (face == 3){
      pos.xyz = pos.zyx;
      pos.xz += vec2(0.5, 0.5);
    }else if (face == 4){
      pos.xyz = pos.yzx;
      pos.zy += 0.5;
    }else if (face == 5){
      pos.xyz = pos.yzx;
      pos.zy += vec2(0.5, -0.5);
    }
  }
  pos.xy += 0.5;
  vec3 model = packedPos + vec3(chunk_offset.x, 0, chunk_offset.y);
  vec4 positionRelativeToCam = view * vec4(pos+model, 1.0);
  gl_Position = proj * positionRelativeToCam;

  float offset = 1.0f/16.0f;
  float blockX = offset*texture_pos.x + offset;
  float blockY = 1-offset*texture_pos.y;
  TexCoord = vec2(aTexCoord.x*blockX + (1-aTexCoord.x)*(blockX-offset),
                  aTexCoord.y*blockY + (1-aTexCoord.y)*(blockY-offset));
  float distanceFog = length(positionRelativeToCam.xyz);

  Visibility = clamp(exp(-pow((distanceFog*density), gradient)), 0.0, 1.0);
  Normal = normals[int(face)];
  FragPos = vec3( vec4(aPos+model, 1.0));
  lightValue = light_value;
}
