#pragma once

#include <glm/vec2.hpp>
#include "voxel/VoxelType.h"
#include "solvers/WaterSolver.h"
#include "solvers/SandSolver.h"

struct VoxelChunk;
struct VoxelChunkViews;
struct VoxelContext;
class Scene;

struct PhysicsDebugData {
    const uint8_t    *heightMap;
    const glm::vec2  *slopeMap;
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
    bool SimulateWater(const VoxelContext &ctx);

    PhysicsDebugData GetDebugData() const;

  private:
    void CreateHeightMap(const VoxelChunkViews& someViews);
    void CreateSlopeMap();
    void CreateBFSSlopeMap();
    glm::ivec2 BreadthFirstSearchSink(int x, int z);
  private:
    uint32_t     myFrameCounter = 0;
    Scene       &myScene;
    VoxelBitset *xyMovedVoxels;
    uint8_t    *myXZHeightMap = nullptr;
    glm::vec2  *myXZSlopeMap  = nullptr;
    glm::vec2 *myXZFDASlopeMap = nullptr;
    uint8_t *myActionMap = nullptr;

    glm::vec2 *myXZBFSSlopeMap = nullptr;

private:
    SandSolver mySandSolver;
    WaterSolver myWaterSolver;
};
