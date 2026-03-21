#pragma once

struct VoxelChunk;
class Scene;
#include "objects/VoxelType.h"

class PhysicsEngine{
public:
  PhysicsEngine(Scene& aScene);
public:
  void SimulateChunk();

private:
  Scene& myScene;
};
