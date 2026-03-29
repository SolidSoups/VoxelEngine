#version 330 core
layout (location=0) out vec3 gPosition;
layout (location=1) out vec3 gNormal;
layout (location=2) out vec4 gAlbedo;

in vec3 FragPos;
flat in int Type;

uniform vec3 uVoxelColors[8];

void main(){
  gPosition = FragPos;
  gNormal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
  gAlbedo = vec4(uVoxelColors[Type], 1.0);
}
