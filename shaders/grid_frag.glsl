#version 330 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color = vec3(1.0, 0.0, 0.0);
uniform vec3 uCameraPos;

void main() {
  float viewDot = dot(normalize(uCameraPos - FragPos), Normal);

  fragColor = vec4(color, clamp(viewDot, 0.0, 1.0));
}
