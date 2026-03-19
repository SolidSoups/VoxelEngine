#pragma once

#include <glm/glm.hpp>

struct AABB {
  glm::ivec3 min;
  glm::ivec3 max;
};

inline AABB createAABB(const glm::ivec3& aBoxA, const glm::ivec3& aBoxB){
  int minX = glm::min(aBoxA.x, aBoxB.x);
  int maxX = glm::max(aBoxA.x, aBoxB.x);
  int minY = glm::min(aBoxA.y, aBoxB.y);
  int maxY = glm::max(aBoxA.y, aBoxB.y);
  int minZ = glm::min(aBoxA.z, aBoxB.z);
  int maxZ = glm::max(aBoxA.z, aBoxB.z);
  return {{minX, minY, minZ}, {maxX, maxY, maxZ}};
}

// calculate if a point falls within the bounds of a AABB defined by two points,
// aBoxA and aBoxB
inline bool withinBounds(const glm::ivec3 &aPoint, const glm::ivec3 &aBoxA,
                         const glm::ivec3 &aBoxB) {
  int minX = glm::min(aBoxA.x, aBoxB.x);
  int maxX = glm::max(aBoxA.x, aBoxB.x);
  int minY = glm::min(aBoxA.y, aBoxB.y);
  int maxY = glm::max(aBoxA.y, aBoxB.y);
  int minZ = glm::min(aBoxA.z, aBoxB.z);
  int maxZ = glm::max(aBoxA.z, aBoxB.z);
  return (aPoint.x >= minX and aPoint.x <= maxX) and
         (aPoint.y >= minY and aPoint.y <= maxY) and
         (aPoint.z >= minZ and aPoint.z <= maxZ);
}
