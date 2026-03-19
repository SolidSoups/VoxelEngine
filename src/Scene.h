#pragma once

#include "objects/VoxelChunk.h"
struct Camera;

class Scene{
public:
  Scene();

  void Render();
  inline VoxelChunk& GetVoxelChunk() { return voxelChunk; }
private:
  VoxelChunk voxelChunk;
};
