#include "CameraController.h"

#include <glm/glm.hpp>

void CameraController::MoveCamera(Camera& camera){
  camera.target = myTarget;  
  camera.worldUp = myUp;
  camera.position.x = myTarget.x + myOrbitDistance * glm::cos(myPitch) * glm::sin(myYaw);
  camera.position.y = myTarget.y + myOrbitDistance * glm::sin(myPitch);
  camera.position.z = myTarget.z + myOrbitDistance * glm::cos(myPitch) * glm::cos(myYaw);
}

void CameraController::Zoom(float aScrollY){
  myOrbitDistance -= aScrollY * myZoomSpeed; 
  myOrbitDistance = glm::clamp(myOrbitDistance, MIN_ZOOM, MAX_ZOOM);
}

void CameraController::Orbit(const glm::vec2& aMouseDelta){
  myYaw += aMouseDelta.x * myOrbitSpeed;
  myPitch += aMouseDelta.y * myOrbitSpeed;
  myPitch = glm::clamp(myPitch, -89.f, 89.f);
}

void CameraController::Pan(const glm::vec2& aMouseDelta){
  myRight = glm::normalize(glm::cross(myForward, myUp));
  myUp = glm::normalize(glm::cross(myRight, myForward));
  myTarget -= myRight * aMouseDelta.x * myPanSpeed;
  myTarget += myUp * aMouseDelta.y * myPanSpeed;
}
