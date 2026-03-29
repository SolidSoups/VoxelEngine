#pragma once

#include "objects/VoxelType.h"

struct VoxelChunk;
class Scene;

struct VoxelContext {
  Voxel *voxels;
  VoxelType type;
  VoxelIndex index;
  glm::ivec3 gridPos;
  VoxelIndex chunkSize;
  VoxelChunk& voxelChunk;
  VoxelBitset *xyMovedVoxels;
};

class PhysicsEngine {
public:
  PhysicsEngine(Scene &aScene);

public:
  void SimulateChunk();
  bool SimulateSand(const VoxelContext &ctx);
  bool SimulateWater(const VoxelContext &ctx);

  bool MoveVoxelStraightDown(const VoxelContext& ctx);
  bool MoveSandDownOnWater(const VoxelContext& ctx);
  bool MoveVoxelDiagonallyDown(const VoxelContext &ctx);
  bool MoveVoxelHorizontally(const VoxelContext &ctx);

  bool GetRandDiagonalNeighbour(const VoxelContext&ctx, VoxelIndex &outIndex);
  bool GetRandHorizontalNeighbour(const VoxelContext&ctx, VoxelIndex &outIndex);

private:
  uint32_t myFrameCounter = 0;
  Scene &myScene;
  VoxelBitset *xyMovedVoxels;
};
