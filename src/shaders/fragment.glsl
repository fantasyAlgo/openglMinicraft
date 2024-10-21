#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main(){
  vec4 texture_color = texture(ourTexture, TexCoord);
  FragColor = texture_color;//vec4(TexCoord.xy, 0.0f, 1.0f);
}
