#include "physics/PhysicsEngine.h"

#include <glm/glm.hpp>
#include "voxel/VoxelChunk.h"
#include "rendering/Scene.h"
#include "voxel/VoxelType.h"

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene) {
  xyMovedVoxels = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]();
}

void PhysicsEngine::SimulateChunk() {

  // We swap which direction on x-z plane we iterate
  // every frame
  myFrameCounter++;
  bool reverseZ = myFrameCounter % 2;
  bool reverseX = (myFrameCounter / 2) % 2;
  bool somethingMoved = false;

  // clear moved voxels
  memset(xyMovedVoxels, 0, CHUNK_SIZE * CHUNK_SIZE * sizeof(VoxelBitset));

  // create src and dst
  const VoxelChunk &src = myScene.GetVoxelChunk();
  const VoxelIndex chunkSize = src.GetWidth();
  VoxelChunk dst{src};

  VoxelContext ctx{src, chunkSize, dst, VoxelType_EMPTY, 0, {}};

  // Iterate for every z, from the bottom to top...
  for (VoxelIndex y = 0; y < chunkSize; y++)
    for (VoxelIndex iz = 0; iz < chunkSize; iz++) {
      VoxelIndex z = reverseZ ? (chunkSize - 1 - iz) : iz;
      for (VoxelIndex ix = 0; ix < chunkSize; ix++) {
        VoxelIndex x = reverseX ? (chunkSize - 1 - ix) : ix;
        // iterate every voxel in this vertical chunk slice...
        ctx.index = x + y * chunkSize + z * chunkSize * chunkSize;
        ctx.voxel = src[ctx.index];

        // skip all empty voxels
        if (ctx.voxel == VoxelType_EMPTY)
          continue;

        ctx.gridPos = getVoxelGridPosition(ctx.index);

        // Simulate voxels
        switch (ctx.voxel) {
        case (VoxelType_SAND):
          if (SimulateSand(ctx)) {
            somethingMoved = true;
          }
          break;
        case (VoxelType_WATER):
          if (SimulateWater(ctx)) {
            somethingMoved = true;
          }
          break;
        }
      }
    }

  if (somethingMoved)
    dst.isDirty = true;

  myScene.CopyChunk(dst);  
}

bool PhysicsEngine::SimulateSand(const VoxelContext &ctx) {
  // Fall through empty
  if (MoveVoxelStraightDown(ctx))
    return true;

  // Sink through water
  if (MoveSandDownOnWater(ctx))
    return true;

  return MoveVoxelDiagonallyDown(ctx);
}

bool PhysicsEngine::SimulateWater(const VoxelContext &ctx) {
  if (MoveVoxelStraightDown(ctx))
    return true;
  if (MoveVoxelDiagonallyDown(ctx))
    return true;

  return MoveVoxelHorizontally(ctx);
}

bool PhysicsEngine::MoveSandDownOnWater(const VoxelContext &ctx) {
  if (ctx.gridPos.y <= 0)
    return false;

  // only move down if water under
  size_t indexUnder = ctx.index - ctx.width;
  if (ctx.src[indexUnder] != VoxelType_WATER)
    return false;
  if (ctx.dst[indexUnder] != ctx.src[indexUnder])
    return false;

  // swap them in the dst
  ctx.dst[ctx.index] = VoxelType_WATER;
  ctx.dst[indexUnder] = VoxelType_SAND;

  return true;
}

bool PhysicsEngine::MoveVoxelStraightDown(const VoxelContext &ctx) {
  // get voxel position, skip bottom
  if (ctx.gridPos.y == 0)
    return false;

  // only move down on empty
  VoxelIndex indexUnder = ctx.index - ctx.width;
  if (ctx.src[indexUnder] != VoxelType_EMPTY)
    return false;
  if (ctx.dst[indexUnder] != ctx.src[indexUnder])
    return false;

  // swap this voxel with the one under in the dst
  ctx.dst[indexUnder] = ctx.src[ctx.index];
  ctx.dst[ctx.index] = ctx.src[indexUnder];
  return true;
}

bool PhysicsEngine::MoveVoxelDiagonallyDown(const VoxelContext &ctx) {
  // we are at the bottom, skip
  if (ctx.gridPos.y == 0)
    return false;

  VoxelIndex chosen;
  if (!GetRandDiagonalNeighbour(ctx, chosen))
    return false;

  // swap
  ctx.dst[chosen] = ctx.src[ctx.index];
  ctx.dst[ctx.index] = ctx.src[chosen]; 

  return true;
}

bool PhysicsEngine::MoveVoxelHorizontally(const VoxelContext &ctx) {
  // Add more spread if above is water ???
  bool aboveIsWater = ctx.gridPos.y < ctx.width - 1 &&
                      ctx.src[ctx.index + ctx.width] == VoxelType_WATER;
  bool weAreWater = ctx.voxel == VoxelType_WATER;
  int spreadChance = aboveIsWater and weAreWater ? 1 : 3;
  if (rand() % spreadChance != 0)
    return false;

  // nothing below, keep falling
  if (ctx.gridPos.y > 0 &&
      ctx.src[ctx.index - ctx.width] == VoxelType_EMPTY)
    return false;

  VoxelIndex chosen;
  if (!GetRandHorizontalNeighbour(ctx, chosen)) {
    return false;
  }
  ctx.dst[ctx.index] = ctx.src[chosen];
  ctx.dst[chosen] = ctx.src[ctx.index];
  return true;
}

bool PhysicsEngine::GetRandDiagonalNeighbour(const VoxelContext &ctx,
                                             VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  // cache index under and the size of a z slice
  const VoxelIndex indexUnder = ctx.index - ctx.width;
  const VoxelIndex zSliceSize = ctx.width * ctx.width;

  // check diagonal left
  if (ctx.gridPos.x > 0 && ctx.src[indexUnder - 1] == VoxelType_EMPTY && ctx.dst[indexUnder - 1] == ctx.src[indexUnder - 1]) {
    candidates[count++] = indexUnder - 1;
  }
  // check diagonal right
  if (ctx.gridPos.x < ctx.width - 1 &&
      ctx.src[indexUnder + 1] == VoxelType_EMPTY && ctx.dst[indexUnder + 1] == ctx.src[indexUnder + 1]) {
    candidates[count++] = indexUnder + 1;
  }
  // check diagonal front
  if (ctx.gridPos.z < ctx.width - 1 &&
      ctx.src[indexUnder + zSliceSize] == VoxelType_EMPTY && ctx.dst[indexUnder + zSliceSize] == ctx.src[indexUnder + zSliceSize]) {
    candidates[count++] = indexUnder + zSliceSize;
  }
  // check diagonal back
  if (ctx.gridPos.z > 0 &&
      ctx.src[indexUnder - zSliceSize] == VoxelType_EMPTY && ctx.dst[indexUnder - zSliceSize] == ctx.src[indexUnder - zSliceSize]) {
    candidates[count++] = indexUnder - zSliceSize;
  }

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random diagonal candidate with this voxel
  outIndex = candidates[rand() % count];
  return true;
}

bool PhysicsEngine::GetRandHorizontalNeighbour(const VoxelContext &ctx,
                                               VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  const VoxelIndex zSliceSize = ctx.width * ctx.width;

  // Horizontally left
  if (ctx.gridPos.x > 0 and ctx.src[ctx.index - 1] == VoxelType_EMPTY and ctx.dst[ctx.index - 1] == ctx.src[ctx.index - 1]) {
    candidates[count++] = ctx.index - 1;
  }
  // Horizontally right
  if (ctx.gridPos.x < ctx.width - 1 and
      ctx.src[ctx.index + 1] == VoxelType_EMPTY and ctx.dst[ctx.index + 1] == ctx.src[ctx.index + 1]) {
    candidates[count++] = ctx.index + 1;
  }
  // Horizontally forward
  if (ctx.gridPos.z < ctx.width - 1 and
      ctx.src[ctx.index + zSliceSize] == VoxelType_EMPTY and ctx.dst[ctx.index + zSliceSize] == ctx.src[ctx.index + zSliceSize]) {
    candidates[count++] = ctx.index + zSliceSize;
  }
  // Horizontally back
  if (ctx.gridPos.z > 0 and
      ctx.src[ctx.index - zSliceSize] == VoxelType_EMPTY and ctx.dst[ctx.index - zSliceSize] == ctx.src[ctx.index - zSliceSize]) {
    candidates[count++] = ctx.index - zSliceSize;
  }

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random horizontal candidate with this voxel
  outIndex = candidates[rand() % count];
  return true;
}
