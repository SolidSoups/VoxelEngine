#pragma once

#include <glm/vec2.hpp>
#include "voxel/VoxelType.h"

struct VoxelChunk;
struct VoxelChunkViews;
struct VoxelContext;
class Scene;

class PhysicsEngine
{
  public:
    PhysicsEngine(Scene &aScene);
    ~PhysicsEngine();

  public:
    void SimulateChunk();
    bool SimulateSand(const VoxelContext &ctx);
    bool SimulateWater(const VoxelContext &ctx);

  private:
    void CreateHeightMap(const VoxelChunkViews& someViews);
    void CreateSlopeMap();
  private:
    uint32_t     myFrameCounter = 0;
    Scene       &myScene;
    VoxelBitset *xyMovedVoxels;
    uint8_t* myXZHeightMap = nullptr;
    glm::vec2* myXZSlopeMap = nullptr;
};
