#version 330 core
layout (location=0) in vec3 aPosition;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
  vec4 worldPos = vec4(aPosition, 1.0);
  FragPos = worldPos.xyz;
  gl_Position = projection * view * worldPos;
}
