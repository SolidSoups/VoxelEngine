#pragma once

struct Camera;

#define MIN_ZOOM 0.1f
#define MAX_ZOOM 10.f

class CameraController{
public:
  void MoveCamera(Camera& camera, float dt);
  void Reset();

  void Zoom(float aScrollY);
  void Orbit(const glm::dvec2& aMouseDelta);
  void Pan(const glm::dvec2& aMouseDelta);
private:
  float myPitch = 45.f, myYaw = -45.f;
  float myOrbitDistance = 5.0f;

  // lerping values
  float myCurrentOrbitDistance = 5.0f;
  float myCurrentPitch = 45.f;
  float myCurrentYaw = 45.f;
  glm::vec3 myCurrentTarget{2.0f};

  glm::vec3 myTarget{2.0f};
  glm::vec3 myForward{0.0f, 0.0f, -1.0f};
  glm::vec3 myUp{0.0f, 1.0f, 0.0f};
  glm::vec3 myRight{-1.0f, 0.0f, 0.0f};

  const float myZoomSpeed = 1.0f;
  const float myOrbitSpeed = 0.5f;
  const float myPanSpeed = 0.01f;
  const float mySmoothSpeed = 0.01f;
};
