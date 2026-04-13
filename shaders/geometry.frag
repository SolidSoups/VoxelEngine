#version 330 core
layout (location=0) out vec3 gPosition;
layout (location=1) out vec3 gNormal;
layout (location=2) out vec4 gAlbedo;

in vec3 FragPos;
flat in int Type;


uniform int uDebugMode;
uniform usampler3D uActionTex;
uniform sampler2D uArrowTex;
uniform sampler2D uDebugTex;
uniform int uChunkSize;
uniform float uChunkWidth;
uniform vec3 uVoxelColors[8];

vec3 ACTION_COLORS[6] = vec3[6](
    vec3(0.15, 0.15, 0.15), // None, dark grey
    vec3(0.1, 0.3, 1.0), // Fall down, blue
    vec3(0.0, 0.8, 1.0), // FallDiagonally, cyan
    vec3(0.0, 0.9, 0.2), // Spread horizontally, green
    vec3(1.0, 1.0, 0.0), // Surface Spread, yellow
    vec3(1.0, 0.4, 0.0) // Sink through water, orange
);



vec4 GetDebugColor(){
    float cellWidth = uChunkWidth / float(uChunkSize);
    vec3 adjFragPos = FragPos + vec3(cellWidth * 0.5);

    if(uDebugMode == 5){
        vec3 faceNormal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
        ivec3 coord = ivec3(floor((adjFragPos - vec3(faceNormal * cellWidth*0.5)) / cellWidth)); 
        coord = clamp(coord, ivec3(0), ivec3(uChunkSize-1));
        uint action = texelFetch(uActionTex, coord, 0).r;
        return vec4(ACTION_COLORS[action], 1.0);
    }

    vec2 normal = adjFragPos.xz  / vec2(uChunkWidth, uChunkWidth);
    vec4 color = texture(uDebugTex, normal);

    // we are sloping, add arrow texture
    if(uDebugMode == 2 || uDebugMode == 3){
        // decode slope direction from red green
        vec2 dir = color.rg * 2.0  - 1.0; // centered 
        // vec2 dir = vec2(1.0, 0.0);
        float halfPi = 1.5708;
        float angle = atan(dir.y, dir.x) + halfPi; // needs to be rotated to face the right direction

        // per-voxel uv
        vec2 uv = fract(adjFragPos.xz  / cellWidth);

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
