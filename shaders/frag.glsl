#version 330 core
out vec4 fragColor;
in vec3 FragPos;
flat in int Type;

uniform vec3 uVoxelColors[8];

void main() {
  fragColor = vec4(uVoxelColors[Type], 1.0);
}
