#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float Visibility;
flat in int lightValue;

uniform sampler2D ourTexture;
uniform vec3 lightPos;

vec4 skyColor = vec4(0.5333, 0.8, 0.858823, 1.0f);

void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

  float ambientStrength = 0.5;
  vec3 ambient = ambientStrength * lightColor;

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);  
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec4 texture_color = vec4((float(lightValue)/15.0f)*(diffuse+ambient), 1.0f)*texture(ourTexture, TexCoord);
  if (texture_color.a < 0.01)
    discard;
  FragColor = mix(skyColor, texture_color, Visibility); //vec4(TexCoord.xy, 0.0f, 1.0f);
}
