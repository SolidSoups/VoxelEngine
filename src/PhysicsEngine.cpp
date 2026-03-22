#include "PhysicsEngine.h"

#include <glm/glm.hpp>
#include "objects/VoxelChunk.h"
#include "Scene.h"

// helper for getting the grid position
glm::ivec3 getGridPosition(size_t aIndex, size_t aChunkSize) {
  return {aIndex % aChunkSize, (aIndex / aChunkSize) % aChunkSize,
          aIndex / (aChunkSize * aChunkSize)};
}
// helper for getting an index at a grid position
size_t getIndex(const glm::ivec3 &aGridPos, size_t aChunkSize) {
  return aGridPos.x + aGridPos.y * aChunkSize +
         aGridPos.z * aChunkSize * aChunkSize;
}

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene) {}

void PhysicsEngine::SimulateChunk() {
  VoxelChunk &chunk = myScene.GetVoxelChunk();
  const voxel_index chunkSize = chunk.GetSize();
  const voxel_index size = chunkSize * chunkSize;

  // Iterate for every z, from the bottom to top...
  for (voxel_index z = 0; z < chunkSize; z++)
    // iterate every voxel in this vertical chunk slice...
    for (voxel_index i = 0; i < size; i++) {
      voxel_index index = i + z * size;
      Voxel &voxel = chunk[index];
      // skip all empty voxels
      if (voxel == VoxelType_EMPTY)
        continue;

      // Create context to share to sub-functions
      VoxelContext ctx{chunk.voxels, index, getGridPosition(index, chunkSize),
                       chunkSize};

      // Simulate voxels
      switch (voxel) {
      case (VoxelType_SAND):
        SimulateSand(ctx);
        break;
      }
    }
}

void PhysicsEngine::SimulateSand(const VoxelContext &ctx) {
  // get voxel position, skip bottom
  if (ctx.gridPos.y == 0)
    return;

  // check if it's empty under
  voxel_index indexUnder = ctx.index - ctx.chunkSize;
  Voxel &voxelUnder = ctx.voxels[indexUnder];

  // if empty, move down and finish
  if (voxelUnder == VoxelType_EMPTY) {
    std::swap(voxelUnder, ctx.voxels[ctx.index]);
    return;
  }

  voxel_index candidates[4];
  size_t count = 0;
  const voxel_index zSliceSize = ctx.chunkSize * ctx.chunkSize;

  // check diagonal left
  if (ctx.gridPos.x > 0 && ctx.voxels[indexUnder - 1] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder - 1;
  }
  // check diagonal right
  if (ctx.gridPos.x < ctx.chunkSize - 1 &&
      ctx.voxels[indexUnder + 1] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder + 1;
  }
  // check diagonal front
  if (ctx.gridPos.z < ctx.chunkSize - 1 &&
      ctx.voxels[indexUnder + zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder + zSliceSize;
  }
  // check diagonal back
  if (ctx.gridPos.z > 0 &&
      ctx.voxels[indexUnder - zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder - zSliceSize;
  }

  // move to a diagonal candidate with equal chance
  if (count > 0) {
    voxel_index chosen = candidates[rand() % count];
    std::swap(ctx.voxels[ctx.index], ctx.voxels[chosen]);
  }
}
