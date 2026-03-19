#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "VoxelType.h"

#define CHUNK_SIZE 16


struct VoxelChunk {
  VoxelChunk() {
    voxels = new Voxel[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    memset(voxels, VoxelType::EMPTY,
           sizeof(Voxel) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
  }
  ~VoxelChunk() { delete[] voxels; }

  // delete copy operator and constructor
  VoxelChunk(const VoxelChunk &) = delete;
  VoxelChunk &operator=(const VoxelChunk &) = delete;

  // define move assignment and operator
  VoxelChunk(VoxelChunk &&other) noexcept : voxels(other.voxels) {
    other.voxels = nullptr;
  }
  VoxelChunk &operator=(VoxelChunk &&other) {
    if (this != &other) {
      delete[] voxels;
      voxels = other.voxels;
      other.voxels = nullptr;
    }
    return *this;
  }

public:
  std::vector<glm::ivec3> getNonEmpty() {
    std::vector<glm::ivec3> result;
    for (int z = 0; z < CHUNK_SIZE; z++)
      for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++) {
          if ((*this)[x, y, z] != EMPTY)
            result.push_back({x, y, z});
        }
    return result;
  }

public:
  // flat array to store voxels
  Voxel *voxels;

  // return a reference, so that this voxel could also be changed
  Voxel &operator[](int x, int y, int z) {
    size_t index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    return voxels[index];
  }

  Voxel &operator[](VoxelIndex index){
    return voxels[index];
  }

  // Query for every voxel index where it matches the predicate
  template<typename Pred>
  void QueryVoxels(Pred aPredicate, std::vector<uint16_t>& outIndices){
    size_t size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    for(uint16_t i=0; i < size; i++){
      if(aPredicate(i, voxels[i])){
        outIndices.push_back(i);
      } 
    }
  }

};
inline glm::ivec3 getVGridPos(VoxelIndex aIndex) {
  glm::ivec3 result;  
  result.x = aIndex % CHUNK_SIZE;
  result.y = (aIndex / CHUNK_SIZE) % CHUNK_SIZE;
  result.z = aIndex / (CHUNK_SIZE * CHUNK_SIZE);
  return result;
}

inline VoxelIndex getVIndex(const glm::ivec3& aGridPosition) {
  return aGridPosition.x + aGridPosition.y * CHUNK_SIZE + aGridPosition.z * CHUNK_SIZE * CHUNK_SIZE; 
}

