#pragma once

#include "objects/Camera.h"

#define MIN_ZOOM 0.1f
#define MAX_ZOOM 10.f

class CameraController{
public:
  void MoveCamera(Camera& camera);

  void Zoom(float aScrollY);
  void Orbit(const glm::vec2& aMouseDelta);
  void Pan(const glm::vec2& aMouseDelta);
private:
  float myPitch = 0.f, myYaw = 0.f;
  float myOrbitDistance = 5.0f;

  glm::vec3 myTarget{0.0f};
  glm::vec3 myForward{0.0f, 0.0f, 1.0f};
  glm::vec3 myUp{0.0f, 1.0f, 0.0f};
  glm::vec3 myRight{1.0f, 0.0f, 0.0f};

  const float myZoomSpeed = 10.f;
  const float myOrbitSpeed = 10.f;
  const float myPanSpeed = 10.0f;
};
