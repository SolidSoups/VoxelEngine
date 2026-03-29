#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

void main(){
  vec3 pos = texture(gPosition, TexCoords).rgb;
  vec3 normal = texture(gNormal, TexCoords).rgb;
  vec3 albedo = texture(gAlbedo, TexCoords).rgb;

  // simple directional light
  vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 ambient = 0.15 * albedo;
  vec3 color = ambient + diff * albedo;

  FragColor = vec4(color, 1.0);
}
