#version 330 core
layout (location=0) out vec3 gPosition;
layout (location=1) out vec3 gNormal;
layout (location=2) out vec4 gAlbedo;

in vec3 FragPos;
flat in int Type;


uniform int uDebugMode;
uniform sampler2D uArrowTex;
uniform sampler2D uDebugTex;
uniform int uChunkSize;
uniform float uChunkWidth;
uniform vec3 uVoxelColors[8];

vec4 GetDebugColor(){
    float cellWidth = uChunkWidth / float(uChunkSize);
    vec2 normal = (FragPos.xz + vec2(cellWidth * 0.5))  / vec2(uChunkWidth, uChunkWidth);
    vec4 color = texture(uDebugTex, normal);

    // we are sloping, add arrow texture
    if(uDebugMode == 2){
        // decode slope direction from red green
        vec2 dir = color.rg * 2.0  - 1.0; // centered 
        // vec2 dir = vec2(1.0, 0.0);
        float halfPi = 1.5708;
        float angle = atan(dir.y, dir.x) + halfPi; // needs to be rotated to face the right direction

        // per-voxel uv
        vec2 uv = fract((FragPos.xz + vec2(cellWidth * 0.5)) / cellWidth);

        // rotate around center
        uv -= 0.5;
        float s = sin(angle), c = cos(angle);
        uv = mat2(c, -s, s, c) * uv + 0.5; // rotate with 2d rotation matrix

        vec4 arrow = texture(uArrowTex, uv);
        color = mix(color, vec4(1.0, 0.0, 0.0, 1.0) * arrow, arrow.a);
    }

    return color;
}

void main(){
  gPosition = FragPos;
  gNormal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

  if(uDebugMode > 0){
      gAlbedo = GetDebugColor();
  }
  else
      gAlbedo = vec4(uVoxelColors[Type], 1.0);
}
