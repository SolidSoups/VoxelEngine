#pragma once

#include <glm/vec2.hpp>
#include "solvers/WaterSolver.h"
#include "solvers/SandSolver.h"

struct VoxelChunk;
struct VoxelChunkViews;
struct VoxelContext;
class Scene;

struct PhysicsDebugData
{
    const uint8_t   *heightMap;
    const glm::vec2 *slopeMap;
    const uint8_t   *actionMap;
    const glm::vec2 *fdaSlopeMap;
    const glm::vec2 *waveVelocityMap;
};
struct PhysicsWaterSettings
{
    float     waveSpeed       = 0.005f; // terrain-slope response strength
    float     waveDamping     = 0.95f;  // velocity decay per frame
    float     ambientStrength = 0.02f;  // sinusoidal wave push strength
    float     diffusion       = 0.4f;   // velocity field smoothing
    float     maxVelocity     = 0.5f;   // velocity magnitude cap
    glm::vec2 waveDirection   = glm::vec2(0.9f, 0.3f);
    float     realWaveSpeed   = 0.3f;  // wave animation speed
    float     waveFrequency   = 0.15f; // spatial wave frequency
    float     surgeSpeed      = 1.5f;
};

class PhysicsEngine
{
  public:
    PhysicsEngine(Scene &aScene);
    ~PhysicsEngine();

  public:
    void SimulateChunk();

  private:
    void       UpdateWaveVelocities();
    glm::vec2 *myXZWaveVelocity = nullptr;
    glm::vec2 *myGradStep       = nullptr;
    glm::vec2 *myWaveStep       = nullptr;
    glm::vec2 *myCohesionStep   = nullptr;

    // Height maps
    void     CreateHeightMap(const VoxelChunkViews &someViews);
    uint8_t *myXZHeightMap = nullptr;

    // Slope maps, maybe irrelevent
    void       CreateSlopeMap();
    glm::vec2 *myXZSlopeMap = nullptr;

    // Breadth first searching
    void       CreateBFSSlopeMap();
    glm::ivec2 BreadthFirstSearchSink(int x, int z);
    glm::vec2 *myXZBFSSlopeMap = nullptr;

  private:
    uint32_t myFrameCounter = 0;
    Scene   &myScene;
    uint8_t *myActionMap = nullptr;

  private:
    SandSolver  mySandSolver;
    WaterSolver myWaterSolver;

  public:
    PhysicsWaterSettings waterSettings;
};
