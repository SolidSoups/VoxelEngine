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
  inline void DeprojectMouseToRay(const glm::ivec2& aScreenMousePos, const glm::ivec2& aScreenSize, glm::vec3& outRayOrigin,glm::vec3& outRayDirection){
    // convert screen mouse to normalized device coordinates
    float ndcX = (2.0f * aScreenMousePos.x) / aScreenSize.x - 1.0f;
    float ndcY = 1.0f - (2.0f * aScreenMousePos.y) / aScreenSize.y; // flipped y
    
    // unproject two points on near plane and far plane 
    glm::mat4 view = GetViewMatrix();
    glm::mat4 projection = GetProjectionMatrix();
    glm::mat4 invVP = glm::inverse(projection * view);

    glm::vec4 nearClip = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farClip = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    // perspective divide ????
    nearClip /= nearClip.w;
    farClip /= farClip.w;

    outRayOrigin = glm::vec3(nearClip);
    outRayDirection = glm::normalize(glm::vec3(farClip - nearClip));
  }
private:
  float myNearPlane = 0.1f;
  float myFarPlane = 1000.f;
  float myFOV = 60.f;
};
