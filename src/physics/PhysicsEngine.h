#pragma once

#include "voxel/VoxelType.h"

struct VoxelChunk;
struct VoxelContext;
class Scene;

class PhysicsEngine {
public:
  PhysicsEngine(Scene &aScene);

public:
  void SimulateChunk();
  bool SimulateSand(const VoxelContext &ctx);
  bool SimulateWater(const VoxelContext &ctx);

private:
  uint32_t myFrameCounter = 0;
  Scene &myScene;
  VoxelBitset *xyMovedVoxels;
};
