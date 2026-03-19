#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
  glm::vec3 position{-2.0f, 4.0f, -2.0f};
  glm::vec3 target{2.0f, 0.0f, 2.0f};
  glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
  float aspectRatio = 1.0f;

  inline glm::mat4 GetViewMatrix() const {
    return glm::lookAt(position, target, worldUp);
  }
  inline glm::mat4 GetProjectionMatrix() const {
    return glm::perspective(glm::radians(myFOV), aspectRatio, myNearPlane, myFarPlane);
  }
private:
  float myNearPlane = 0.1f;
  float myFarPlane = 1000.f;
  float myFOV = 60.f;
};
