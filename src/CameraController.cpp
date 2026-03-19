#include "CameraController.h"

#include <glm/glm.hpp>
#include "objects/Camera.h"

#include <print>

void CameraController::MoveCamera(Camera& aCamera, float aDeltaTime){
  const float TARGET_SPEED = 20.0f;
  const float ROTATION_SPEED = 15.0f;
  const float DIST_SPEED = 8.0f;
  float targetSpeed = 1.0f - glm::exp(-TARGET_SPEED * aDeltaTime);
  float rotationSpeed = 1.0f - glm::exp(-ROTATION_SPEED * aDeltaTime);
  float distanceSpeed = 1.0f - glm::exp(-DIST_SPEED * aDeltaTime);

  myCurrentPitch = glm::mix(myCurrentPitch, myPitch, rotationSpeed);
  myCurrentYaw = glm::mix(myCurrentYaw, myYaw, rotationSpeed);
  myCurrentOrbitDistance = glm::mix(myCurrentOrbitDistance, myOrbitDistance, distanceSpeed);
  myCurrentTarget = glm::mix(myCurrentTarget, myTarget, targetSpeed);

  float pitchRadians = glm::radians(myCurrentPitch);
  float yawRadians = glm::radians(myCurrentYaw);

  glm::vec3 desiredPos;
  desiredPos.x = myCurrentTarget.x + myCurrentOrbitDistance * glm::cos(pitchRadians) * glm::sin(yawRadians);
  desiredPos.y = myCurrentTarget.y + myCurrentOrbitDistance * glm::sin(pitchRadians);
  desiredPos.z = myCurrentTarget.z + myCurrentOrbitDistance * glm::cos(pitchRadians) * glm::cos(yawRadians);

  aCamera.position = desiredPos;
  aCamera.target = myCurrentTarget;
}

void CameraController::Reset(){
  myPitch = 45.f;
  myYaw = 45.f;
  myOrbitDistance = 5.0f;
  myTarget = glm::vec3(2.0f);
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
