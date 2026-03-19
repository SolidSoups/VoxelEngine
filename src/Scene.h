#pragma once

#include "objects/VoxelChunk.h"
struct Camera;

class Scene{
public:
  Scene();

  void Render();
private:
  VoxelChunk voxelChunk;
};
