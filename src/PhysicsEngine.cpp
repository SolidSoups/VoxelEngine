#include "PhysicsEngine.h"

#include <glm/glm.hpp>
#include "objects/VoxelChunk.h"
#include "Scene.h"
#include "objects/VoxelType.h"

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene) {
  xyMovedVoxels = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]();
}

void PhysicsEngine::SimulateChunk() {
  VoxelChunk &chunk = myScene.GetVoxelChunk();
  const VoxelIndex chunkSize = chunk.GetSize();

  // We swap which direction on x-z plane we iterate
  // every frame
  myFrameCounter++;
  bool reverseZ = myFrameCounter % 2;
  bool reverseX = (myFrameCounter / 2) % 2;
  bool somethingMoved = false;

  // clear moved voxels
  memset(xyMovedVoxels, 0, CHUNK_SIZE * CHUNK_SIZE * sizeof(VoxelBitset));

  // Iterate for every z, from the bottom to top...
  for (VoxelIndex y = 0; y < chunkSize; y++)
    for (VoxelIndex zz = 0; zz < chunkSize; zz++) {
      VoxelIndex z = reverseZ ? (chunkSize - 1 - zz) : zz;
      for (VoxelIndex xx = 0; xx < chunkSize; xx++) {
        // iterate every voxel in this vertical chunk slice...
        VoxelIndex x = reverseX ? (chunkSize - 1 - xx) : xx;
        VoxelIndex index = x + y * chunkSize + z * chunkSize * chunkSize;
        Voxel &voxel = chunk.GetVoxel(index);
        // skip all empty voxels
        if (voxel == VoxelType_EMPTY)
          continue;
        if((xyMovedVoxels[x + y * CHUNK_SIZE] & (VoxelBitset(1) << z)) != VoxelBitset(0))
          continue;

        // Create context to share to sub-functions
        VoxelContext ctx{chunk.xyzVoxels, (VoxelType)voxel, index, getVoxelGridPosition(index),
                         chunkSize, chunk, xyMovedVoxels};

        // Simulate voxels
        switch (voxel) {
        case (VoxelType_SAND):
          if (SimulateSand(ctx)){
              somethingMoved = true;
            }
          break;
        case (VoxelType_WATER):
          if (SimulateWater(ctx)){
              somethingMoved = true;
            }
        }
      }
    }

  if (somethingMoved)
    chunk.isDirty = true;
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
  if(MoveVoxelStraightDown(ctx))
    return true; 
  if(MoveVoxelDiagonallyDown(ctx))
    return true;

  return MoveVoxelHorizontally(ctx);
}

bool PhysicsEngine::MoveSandDownOnWater(const VoxelContext &ctx) {
  if (ctx.gridPos.y <= 0)
    return false;

  VoxelIndex indexUnder = ctx.index - ctx.chunkSize;
  if (ctx.voxels[indexUnder] != VoxelType_WATER)
    return false;

  // // Swap with horizontal neighbour first
  // VoxelIndex horNeighbour;
  // if(GetRandHorizontalNeighbour(ctx, horNeighbour)){
  //   ctx.voxelChunk.SwapVoxels(indexUnder, horNeighbour);
  //   return true;
  // }

  // ctx.voxelChunk.SwapVoxels(ctx.index, indexUnder);
  ctx.voxelChunk.SetVoxel(ctx.index, VoxelType_WATER);
  ctx.voxelChunk.SetVoxel(indexUnder, VoxelType_SAND);

  glm::ivec3 sandDest = getVoxelGridPosition(indexUnder);
  ctx.xyMovedVoxels[sandDest.x + sandDest.y * CHUNK_SIZE] |= (VoxelBitset(1) << sandDest.z);
  ctx.xyMovedVoxels[ctx.gridPos.x + ctx.gridPos.y * CHUNK_SIZE] |= (VoxelBitset(1) << ctx.gridPos.z);
  return true;
}

bool PhysicsEngine::MoveVoxelStraightDown(const VoxelContext &ctx) {
  // get voxel position, skip bottom
  if (ctx.gridPos.y == 0)
    return false;

  // check if it's empty under
  VoxelIndex indexUnder = ctx.index - ctx.chunkSize;
  Voxel &voxelUnder = ctx.voxels[indexUnder];

  // if not empty, return
  if (voxelUnder != VoxelType_EMPTY)
    return false;

  // swap this voxel with the one under
  ctx.voxelChunk.SwapVoxels(indexUnder, ctx.index);

  // mark destination as moved
  glm::ivec3 dest = getVoxelGridPosition(indexUnder);
  ctx.xyMovedVoxels[dest.x + dest.y * CHUNK_SIZE] |= (VoxelBitset(1) << dest.z);
  return true;
}

bool PhysicsEngine::MoveVoxelDiagonallyDown(const VoxelContext &ctx) {
  // we are at the bottom, skip
  if (ctx.gridPos.y == 0)
    return false;

  VoxelIndex chosen;
  if (!GetRandDiagonalNeighbour(ctx, chosen))
    return false;

  ctx.voxelChunk.SwapVoxels(chosen, ctx.index);

  // mark destination as moved
  glm::ivec3 dest = getVoxelGridPosition(chosen);
  ctx.xyMovedVoxels[dest.x + dest.y * CHUNK_SIZE] |= (VoxelBitset(1) << dest.z);
  return true;
}

bool PhysicsEngine::MoveVoxelHorizontally(const VoxelContext &ctx) {
  // Add more spread if above is water ???
  bool aboveIsWater = ctx.gridPos.y < ctx.chunkSize -1 &&
    ctx.voxels[ctx.index + ctx.chunkSize] == VoxelType_WATER;
  bool weAreWater = ctx.type == VoxelType_WATER;
  int spreadChance = aboveIsWater and weAreWater ? 1 : 3;
  if (rand() % spreadChance != 0)
    return false;

  // nothing below, keep falling
  if (ctx.gridPos.y > 0 &&
      ctx.voxels[ctx.index - ctx.chunkSize] == VoxelType_EMPTY)
    return false;

  VoxelIndex chosen;
  if (!GetRandHorizontalNeighbour(ctx, chosen)){
    return false;
  }
  ctx.voxelChunk.SwapVoxels(ctx.index, chosen);

  // mark destination as moved
  glm::ivec3 dest = getVoxelGridPosition(chosen);
  ctx.xyMovedVoxels[dest.x + dest.y * CHUNK_SIZE] |= (VoxelBitset(1) << dest.z);
  return true;
}

bool PhysicsEngine::GetRandDiagonalNeighbour(const VoxelContext &ctx,
                                             VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  // cache index under and the size of a z slice
  const VoxelIndex indexUnder = ctx.index - ctx.chunkSize;
  const VoxelIndex zSliceSize = ctx.chunkSize * ctx.chunkSize;

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
  outIndex = candidates[rand() % count];
  return true;
}

bool PhysicsEngine::GetRandHorizontalNeighbour(const VoxelContext &ctx,
                                               VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  const VoxelIndex zSliceSize = ctx.chunkSize * ctx.chunkSize;

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
  outIndex = candidates[rand() % count];
  return true;
}

