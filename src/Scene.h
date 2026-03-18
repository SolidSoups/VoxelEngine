#pragma once

#include "objects/VoxelChunk.h"

class Scene{
public:
  Scene();

  void Render();
private:
  VoxelChunk voxelChunk;
};
