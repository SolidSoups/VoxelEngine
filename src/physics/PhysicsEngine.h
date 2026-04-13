#pragma once

#include <glm/vec2.hpp>
#include "voxel/VoxelType.h"

struct VoxelChunk;
struct VoxelChunkViews;
struct VoxelContext;
class Scene;

struct PhysicsDebugData {
    const uint8_t    *heightMap;
    const glm::vec2  *slopeMap;
    const VoxelBitset *movedVoxels;
    const uint8_t *actionMap;
    const glm::vec2 *fdaSlopeMap;
};

class PhysicsEngine
{
  public:
    PhysicsEngine(Scene &aScene);
    ~PhysicsEngine();

  public:
    void SimulateChunk();
    bool SimulateSand(const VoxelContext &ctx);
    bool SimulateWater(const VoxelContext &ctx);

    PhysicsDebugData GetDebugData() const;

  private:
    void CreateHeightMap(const VoxelChunkViews& someViews);
    void CreateSlopeMap();
    void CreateSlopeMap_fda_d8(const VoxelBitset* xzOccupancy);
    glm::vec2 FDAFindSteepestSlope(int x, int z, int maxSteps);
    // Returns the final height of simulating FDA D8 steps from a point
    int FDASimulateSteps(int x, int z, int height, const VoxelBitset* xzOccupancy,int maxSteps = 5);
  private:
    uint32_t     myFrameCounter = 0;
    Scene       &myScene;
    VoxelBitset *xyMovedVoxels;
    uint8_t    *myXZHeightMap = nullptr;
    glm::vec2  *myXZSlopeMap  = nullptr;
    glm::vec2 *myXZFDASlopeMap = nullptr;
    uint8_t *myActionMap = nullptr;
};
