#version 330 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 redColor = vec3(1.0, 0.0, 0.0);
uniform vec3 greenColor = vec3(0.0, 1.0, 0.0);
uniform vec3 blueColor = vec3(0.0, 0.0, 1.0);
uniform vec3 grayColor = vec3(0.2, 0.2, 0.2);
uniform vec3 uCameraPos;

bool approx(float x, float y){
  const float pm = 0.13f;
  return
    x > y - pm && x < y + pm;
}

void main() {
  float viewDot = dot(normalize(uCameraPos - FragPos), Normal);
  vec3 color = grayColor;
  if(approx(FragPos.x, 0.0) && approx(FragPos.y, 0.0))
    color = redColor;
  else if(approx(FragPos.z, 0.0) && approx(FragPos.y, 0.0))
    color = blueColor;
  else if(approx(FragPos.x, 0.0) && approx(FragPos.z, 0.0))
    color = greenColor;

  fragColor = vec4(color, clamp(viewDot, 0.0, 1.0));
}
