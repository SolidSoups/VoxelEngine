#pragma once

#include <cassert>
#include <vector>
#include "VoxelType.h"

struct VoxelChunk{
  std::vector<Voxel> xyzVoxels;
  bool isDirty=false;

  VoxelChunk(size_t aWidth) : xyzVoxels(aWidth * aWidth * aWidth), myWidth(aWidth){
  }

  // direct access via index
  Voxel& operator[](size_t index){
    assert(index < myWidth * myWidth * myWidth and "Index is out of bounds!");
    return xyzVoxels[index];
  }
  Voxel& operator[](size_t x, size_t y, size_t z){
    assert(x < myWidth and y < myWidth and z < myWidth and "Index is out of bounds!");
    return xyzVoxels[x + y * myWidth + z * myWidth * myWidth];
  }
  const Voxel& operator[](size_t index) const{
    assert(index < myWidth * myWidth * myWidth and "Index is out of bounds!");
    return xyzVoxels[index];
  }
  const Voxel& operator[](size_t x, size_t y, size_t z) const{
    assert(x < myWidth and y < myWidth and z < myWidth and "Index is out of bounds!");
    return xyzVoxels[x + y * myWidth + z * myWidth * myWidth];
  }
  inline const size_t GetWidth() const { return myWidth; }

  inline const glm::mat4 GetTransform() const{
    const auto scale = glm::scale(glm::mat4(1), glm::vec3(4.0f / myWidth));
    return scale;
  }
private:
  size_t myWidth=0;
};

