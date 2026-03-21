#pragma once

#include "objects/VoxelChunk.h"
struct Camera;

class Scene{
public:
  Scene();

  // delete copy
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  // move
  Scene(Scene&& other) noexcept 
  : voxelChunk(std::move(other.voxelChunk))
  {}
  Scene& operator=(Scene&& other) noexcept {
    if(this != &other){
      voxelChunk = std::move(other.voxelChunk);
    }
    return *this;
  }

  void Render();
  inline VoxelChunk& GetVoxelChunk() { return voxelChunk; }
private:
  VoxelChunk voxelChunk;
};
