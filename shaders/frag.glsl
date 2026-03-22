#version 330 core
out vec4 fragColor;
in vec3 FragPos;

uniform vec3 uVoxelColors[8];
uniform int uVoxelType;

void main() {
  fragColor = vec4(uVoxelColors[uVoxelType], 1.0);
}
