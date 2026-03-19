#include "CameraController.h"

#include <glm/glm.hpp>
#include "objects/Camera.h"

void CameraController::MoveCamera(Camera& camera, float dt){
  float pitchRadians = glm::radians(myPitch);
  float yawRadians = glm::radians(myYaw);

  glm::vec3 desiredPos;
  desiredPos.x = myTarget.x + myOrbitDistance * glm::cos(pitchRadians) * glm::sin(yawRadians);
  desiredPos.y = myTarget.y + myOrbitDistance * glm::sin(pitchRadians);
  desiredPos.z = myTarget.z + myOrbitDistance * glm::cos(pitchRadians) * glm::cos(yawRadians);

  float t = 1.0f - glm::exp(-mySmoothSpeed * dt);
  camera.position = glm::mix(camera.position, desiredPos, t);
  camera.target = glm::mix(camera.target, myTarget, t);
}

void CameraController::Zoom(float aScrollY){
  myOrbitDistance -= aScrollY * myZoomSpeed; 
  myOrbitDistance = glm::clamp(myOrbitDistance, MIN_ZOOM, MAX_ZOOM);
}

void CameraController::Orbit(const glm::dvec2& aMouseDelta){
  myYaw -= aMouseDelta.x * myOrbitSpeed;
  myPitch += aMouseDelta.y * myOrbitSpeed;
  myPitch = glm::clamp(myPitch, -89.f, 89.f);
}

void CameraController::Pan(const glm::dvec2& aMouseDelta){
  float pitchRad = glm::radians(myPitch);
  float yawRad = glm::radians(myYaw);

  myForward.x = -glm::cos(pitchRad) * glm::sin(yawRad);
  myForward.y = -glm::sin(pitchRad);
  myForward.z = -glm::cos(pitchRad) * glm::cos(yawRad);

  myRight = glm::normalize(glm::cross(myForward, glm::vec3(0.0f, 1.0f, 0.0f)));
  myUp = glm::normalize(glm::cross(myRight, myForward));

  myTarget -= myRight * (float)aMouseDelta.x * myPanSpeed;
  myTarget += myUp * (float)aMouseDelta.y * myPanSpeed;
}
