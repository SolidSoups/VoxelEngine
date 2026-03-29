#version 330 core
layout (location=0) in vec3 aPosition;
layout (location=1) in float aType;

out vec3 FragPos;
flat out int Type;

uniform mat4 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  vec4 worldPos = uTransform * vec4(aPosition, 1.0);
  FragPos = worldPos.xyz;
  Type = int(aType);
  gl_Position = uProjection * uView * worldPos;
}
