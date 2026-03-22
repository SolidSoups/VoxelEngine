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

  // We swap which direction on x-z plane we iterate
  // every frame
  myFrameCounter++;
  bool reverseZ = myFrameCounter % 2;
  bool reverseX = (myFrameCounter / 2) % 2;

  // Iterate for every z, from the bottom to top...
  for (voxel_index y = 0; y < chunkSize; y++)
    for (voxel_index zz = 0; zz < chunkSize; zz++) {
      voxel_index z = reverseZ ? (chunkSize - 1 - zz) : zz;
      for (voxel_index xx = 0; xx < chunkSize; xx++) {
        // iterate every voxel in this vertical chunk slice...
        voxel_index x = reverseX ? (chunkSize - 1 - xx) : xx;
        voxel_index index = x + y * chunkSize + z * chunkSize * chunkSize;
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
        case (VoxelType_WATER):
          SimulateWater(ctx);
        }
      }
    }
}

void PhysicsEngine::SimulateSand(const VoxelContext &ctx) {
  if (MoveVoxelStraightDown(ctx))
    return;

  MoveVoxelDiagonallyDown(ctx);
}

void PhysicsEngine::SimulateWater(const VoxelContext &ctx) {
  if (MoveVoxelStraightDown(ctx))
    return;

  // Collect candidates for diagonal movement
  if (MoveVoxelDiagonallyDown(ctx))
    return;

  MoveVoxelHorizontally(ctx);
}

bool PhysicsEngine::MoveVoxelStraightDown(const VoxelContext &ctx) {
  // get voxel position, skip bottom
  if (ctx.gridPos.y == 0)
    return false;

  // check if it's empty under
  voxel_index indexUnder = ctx.index - ctx.chunkSize;
  Voxel &voxelUnder = ctx.voxels[indexUnder];

  // if not empty, return
  if (voxelUnder != VoxelType_EMPTY)
    return false;

  // swap this voxel with the one under
  std::swap(voxelUnder, ctx.voxels[ctx.index]);
  return true;
}

bool PhysicsEngine::MoveVoxelDiagonallyDown(const VoxelContext &ctx) {
  // we are at the bottom, skip
  if (ctx.gridPos.y == 0)
    return false;

  // collect candidates for diagonal down travel
  voxel_index candidates[4];
  size_t count = 0;

  // cache index under and the size of a z slice
  const voxel_index indexUnder = ctx.index - ctx.chunkSize;
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

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random diagonal candidate with this voxel
  voxel_index chosen = candidates[rand() % count];
  std::swap(ctx.voxels[ctx.index], ctx.voxels[chosen]);
  return true;
}

bool PhysicsEngine::MoveVoxelHorizontally(const VoxelContext &ctx) {
  // collect candidates for diagonal down travel
  voxel_index candidates[4];
  size_t count = 0;

  const voxel_index zSliceSize = ctx.chunkSize * ctx.chunkSize;

  // Horizontally left
  if (ctx.gridPos.x > 0 and ctx.voxels[ctx.index - 1] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index - 1;
  }
  // Horizontally right
  if (ctx.gridPos.x < ctx.chunkSize - 1 and
      ctx.voxels[ctx.index + 1] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index + 1;
  }
  // Horizontally forward
  if (ctx.gridPos.z < ctx.chunkSize - 1 and
      ctx.voxels[ctx.index + zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index + zSliceSize;
  }
  // Horizontally back
  if (ctx.gridPos.z > 0 and
      ctx.voxels[ctx.index - zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index - zSliceSize;
  }

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random horizontal candidate with this voxel
  voxel_index chosen = candidates[rand() % count];
  std::swap(ctx.voxels[ctx.index], ctx.voxels[chosen]);
  return true;
}
