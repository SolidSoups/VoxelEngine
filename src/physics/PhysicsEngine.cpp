#include "physics/PhysicsEngine.h"

#include <glm/glm.hpp>
#include "voxel/VoxelChunk.h"
#include "rendering/Scene.h"
#include "voxel/VoxelType.h"

#include "VoxelPhysics.h"

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


  // Iterate for every z, from the bottom to top...
  for (VoxelIndex y = 0; y < chunkSize; y++)
    for (VoxelIndex iz = 0; iz < chunkSize; iz++) {
      VoxelIndex z = reverseZ ? (chunkSize - 1 - iz) : iz;
      for (VoxelIndex ix = 0; ix < chunkSize; ix++) {
        VoxelIndex x = reverseX ? (chunkSize - 1 - ix) : ix;
        // iterate every voxel in this vertical chunk slice...
        size_t index = x + y * chunkSize + z * chunkSize * chunkSize;
        Voxel voxel = src[index];

        // skip all empty voxels
        if (voxel == VoxelType_EMPTY)
          continue;

        // create context
        VoxelContext ctx{
          .dst = dst, 
          .src = src,
          .width = chunkSize,
          .voxel = voxel, 
          .index = x + y*chunkSize + z * chunkSize * chunkSize, 
          .gridPos = getVoxelGridPosition(index)};

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
  if (FallDown(ctx))
    return true;

  // Sink through water
  if (SinkThroughWater(ctx))
    return true;

  return FallDiagonally(ctx);
}

bool PhysicsEngine::SimulateWater(const VoxelContext &ctx) {
  if (FallDown(ctx))
    return true;
  if (FallDiagonally(ctx))
    return true;

  return SpreadHorizontally(ctx);
}

