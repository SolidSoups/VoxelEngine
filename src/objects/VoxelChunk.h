#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "VoxelType.h"

struct VoxelChunk {
  VoxelChunk();
  ~VoxelChunk();

  // delete copy operator and constructor
  VoxelChunk(const VoxelChunk &) = delete;
  VoxelChunk &operator=(const VoxelChunk &) = delete;

  // define move assignment and operator
  VoxelChunk(VoxelChunk &&other) noexcept;
  VoxelChunk &operator=(VoxelChunk &&other);

public:
  // SETTERS
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

  // GETTERS
  inline Voxel &GetVoxel(const glm::ivec3 &aGridPos) {
    voxel_index index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                        aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    return voxels[index];
  }
  inline Voxel &GetVoxel(voxel_index aIndex) { return voxels[aIndex]; }

  // Returns the side length of the voxel chunk
  inline voxel_index GetSize() const { return CHUNK_SIZE; }

  // we'll work on this
  std::vector<glm::ivec3> getNonEmpty();

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
