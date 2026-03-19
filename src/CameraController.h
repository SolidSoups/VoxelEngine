#pragma once

struct Camera;

#define MIN_ZOOM 0.1f
#define MAX_ZOOM 10.f

class CameraController{
public:
  void MoveCamera(Camera& camera, float dt);

  void Zoom(float aScrollY);
  void Orbit(const glm::dvec2& aMouseDelta);
  void Pan(const glm::dvec2& aMouseDelta);
private:
  float myPitch = 0.f, myYaw = 0.f;
  float myOrbitDistance = 5.0f;

  glm::vec3 myTarget{0.0f};
  glm::vec3 myForward{0.0f, 0.0f, -1.0f};
  glm::vec3 myUp{0.0f, 1.0f, 0.0f};
  glm::vec3 myRight{-1.0f, 0.0f, 0.0f};

  const float myZoomSpeed = 1.0f;
  const float myOrbitSpeed = 0.5f;
  const float myPanSpeed = 0.01f;
  const float mySmoothSpeed = 0.5f;
};
