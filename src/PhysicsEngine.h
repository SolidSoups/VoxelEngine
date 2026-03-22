#pragma once


#include "objects/VoxelType.h"

struct VoxelChunk;
class Scene;

struct VoxelContext {
  Voxel *voxels;
  voxel_index index;
  glm::ivec3 gridPos;
  voxel_index chunkSize;
};

class PhysicsEngine {
public:
  PhysicsEngine(Scene &aScene);

public:
  void SimulateChunk();
  void SimulateSand(const VoxelContext& ctx);

    private : Scene &myScene;
};
