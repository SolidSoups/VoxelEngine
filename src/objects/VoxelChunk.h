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
    VoxelIndex index = aX + aY * CHUNK_SIZE + aZ * CHUNK_SIZE * CHUNK_SIZE;
    voxels[index] = aType;

    // also set cache here
    UpdateBitsets(aX, aY, aZ, aType != VoxelType_EMPTY);
  }
  inline void SetVoxel(const glm::ivec3 &aGridPos, VoxelType aType) {
    VoxelIndex index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                        aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    voxels[index] = aType;

    // also set cache here
    UpdateBitsets(aGridPos.x, aGridPos.y, aGridPos.z, aType != VoxelType_EMPTY);
  }
  inline void SetVoxel(VoxelIndex aIndex, VoxelType aType) {
    voxels[aIndex] = aType;

    // also set cache here, decompose coordinates
    glm::vec3 co = getVoxelGridPosition(aIndex);
    UpdateBitsets(co.x, co.y, co.z, aType != VoxelType_EMPTY);
  }

  // GETTERS
  inline Voxel &GetVoxel(const glm::ivec3 &aGridPos) {
    VoxelIndex index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                        aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    return voxels[index];
  }
  inline Voxel &GetVoxel(VoxelIndex aIndex) { return voxels[aIndex]; }

  inline void SwapVoxels(VoxelIndex aIndex1, VoxelIndex aIndex2){
    std::swap(voxels[aIndex1], voxels[aIndex2]); 
    glm::vec3 p1 = getVoxelGridPosition(aIndex1);
    glm::vec3 p2 = getVoxelGridPosition(aIndex2);
    UpdateBitsets(p1.x, p1.y, p1.z, voxels[aIndex1] != VoxelType_EMPTY);
    UpdateBitsets(p2.x, p2.y, p2.z, voxels[aIndex2] != VoxelType_EMPTY);
  }

  // Returns the side length of the voxel chunk
  inline VoxelIndex GetSize() const { return CHUNK_SIZE; }

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

  uint64_t CountNonEmptyVoxels() const;

private:
  inline void UpdateBitsets(int aX, int aY, int aZ, bool isSolid){
    if(isSolid){
      xRows[aZ + aY * CHUNK_SIZE] |= 1 << aX;
      yColumns[aX + aZ * CHUNK_SIZE] |= 1 << aY;
      zRows[aX + aY * CHUNK_SIZE] |= 1 << aZ;
    }
    else{
      xRows[aZ + aY * CHUNK_SIZE] &= ~(1 << aX);
      yColumns[aX + aZ * CHUNK_SIZE] &= ~(1 << aY);
      zRows[aX + aY * CHUNK_SIZE] &= ~(1 << aZ);
    }
  }

public:
  // Stores every voxel in a flat array
  Voxel *voxels;

  // Stores every EMPTY/NON-EMPTY cell in +x
  VoxelBitset *xRows;
  // Stores every EMPTY/NON-EMPTY cell in +y
  VoxelBitset *yColumns;
  // Stores every EMPTY/NON-EMPTY cell in +z
  VoxelBitset *zRows;

  bool isDirty = false;
};
