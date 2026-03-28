#pragma once

#include "objects/VoxelType.h"

struct VoxelChunk;
class Scene;

struct VoxelContext {
  Voxel *voxels;
  VoxelIndex index;
  glm::ivec3 gridPos;
  VoxelIndex chunkSize;
  VoxelChunk& voxelChunk;
};

class PhysicsEngine {
public:
  PhysicsEngine(Scene &aScene);

public:
  void SimulateChunk();
  bool SimulateSand(const VoxelContext &ctx);
  bool SimulateWater(const VoxelContext &ctx);

  bool MoveVoxelStraightDown(const VoxelContext& ctx);
  bool MoveVoxelDiagonallyDown(const VoxelContext &ctx);
  bool MoveVoxelHorizontally(const VoxelContext &ctx);

private:
  uint32_t myFrameCounter = 0;
  Scene &myScene;
};
