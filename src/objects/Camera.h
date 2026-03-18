#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
  glm::vec3 position{-2.0f, 2.0f, -2.0f};
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

  inline glm::mat4 getViewMatrix() const {
    return glm::lookAt(position, target, worldUp);
  }
  inline glm::mat4 getProjectionMatrix(float aAspectRatio) const {
    return glm::perspective(glm::radians(myFOV), aAspectRatio, myNearPlane, myFarPlane);
  }
private:
  float myNearPlane = 0.1f;
  float myFarPlane = 1000.f;
  float myFOV = 60.f;
};
