#include "PhysicsEngine.h"

#include <glm/glm.hpp>
#include "objects/VoxelChunk.h"
#include "Scene.h"

glm::ivec3 getGridPosition(size_t aIndex, size_t aChunkSize) {
  return {aIndex % aChunkSize, (aIndex / aChunkSize) % aChunkSize,
          aIndex / (aChunkSize * aChunkSize)};
}
size_t getIndex(const glm::ivec3 &aGridPos, size_t aChunkSize) {
  return aGridPos.x + aGridPos.y * aChunkSize +
         aGridPos.z * aChunkSize * aChunkSize;
}
size_t getIndexForward(size_t aCurrentIndex, const glm::ivec3& aGridPos, size_t aChunkSize){
  return aCurrentIndex + aGridPos.x + aGridPos.y * aChunkSize + aGridPos.z * aChunkSize * aChunkSize; 
}
bool isValid(size_t aIndex, size_t aChunkSize){
  return aIndex >= 0 and aIndex < aChunkSize;
}

PhysicsEngine::PhysicsEngine(Scene& aScene)
: myScene(aScene) {}

void PhysicsEngine::SimulateChunk() {
  VoxelChunk& chunk = myScene.GetVoxelChunk();
  const size_t chunkSize = chunk.GetSize();
  const size_t size = chunkSize * chunkSize;

  for (size_t z = 0; z < chunkSize; z++)
    for (size_t i = 0; i < size; i++) {
      size_t index = i + z * size;
      Voxel& voxel = chunk[index];

      if(voxel == VoxelType_SAND){
        // get voxel position, skip bottom
        glm::vec3 pos = getGridPosition(index, chunkSize);
        if(pos.y == 0)
          continue;

        // check if it's empty under
        size_t indexUnder= index - chunkSize;
        Voxel &voxelUnder = chunk[indexUnder];

        // if empty, move down
        if(voxelUnder == VoxelType_EMPTY){
          std::swap(voxelUnder, voxel); 
          continue;
        }
      }
    }
}

