#version 330 core
layout(location=0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  vec4 worldPos = uTransform * vec4(aPos, 1.0);
  FragPos = worldPos.xyz;
  gl_Position = uProjection * uView * worldPos;
}
