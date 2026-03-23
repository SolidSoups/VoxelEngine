#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "VoxelType.h"

struct VoxelChunk {
  VoxelChunk() {
    voxels = new Voxel[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    memset(voxels, VoxelType::VoxelType_EMPTY,
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

  inline void SetVoxel(int aX, int aY, int aZ, VoxelType aType) {
    voxel_index index = aX + aY * CHUNK_SIZE + aZ * CHUNK_SIZE * CHUNK_SIZE;
    voxels[index] = aType;

    // also set cache here
  }
  inline void SetVoxel(const glm::ivec3 &aGridPos, VoxelType aType) {
    voxel_index index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                        aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    voxels[index] = aType;

    // also set cache here
  }
  inline void SetVoxel(voxel_index aIndex, VoxelType aType) {
    voxels[aIndex] = aType;

    // also set cache here
  }
  inline Voxel &GetVoxel(const glm::ivec3 &aGridPos) {
    voxel_index index = aGridPos.x + aGridPos.y * CHUNK_SIZE + aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    return voxels[index];
  }
  inline Voxel &GetVoxel(voxel_index aIndex) { return voxels[aIndex]; }

  // returns the length of every side of the voxel chunk
  inline voxel_index GetSize() const { return CHUNK_SIZE; }

  // we'll work on this
  std::vector<glm::ivec3> getNonEmpty() {
    std::vector<glm::ivec3> result;
    for (int z = 0; z < CHUNK_SIZE; z++)
      for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++) {
          if (GetVoxel({x, y, z}) != VoxelType_EMPTY)
            result.push_back({x, y, z});
        }
    return result;
  }

  // I know you. I don't like you. -- Joe Biden
  template <typename Pred>
  void QueryVoxels(Pred aPredicate, std::vector<uint16_t> &outIndices) {
    size_t size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    for (uint16_t i = 0; i < size; i++) {
      if (aPredicate(i, voxels[i])) {
        outIndices.push_back(i);
      }
    }
  }

public:
  Voxel *voxels;
};
