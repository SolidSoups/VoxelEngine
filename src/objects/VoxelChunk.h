#pragma once

#include <print>
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
    xyzVoxels[index] = aType;

    // also set cache here
    UpdateVoxelCache(aX, aY, aZ, aType); 
    UpdateBitsets(aX, aY, aZ, aType != VoxelType_EMPTY);
  }
  inline void SetVoxel(const glm::ivec3 &aGridPos, VoxelType aType) {
    VoxelIndex index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                       aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    xyzVoxels[index] = aType;

    // also set cache here
    UpdateVoxelCache(aGridPos.x, aGridPos.y, aGridPos.z, aType); 
    UpdateBitsets(aGridPos.x, aGridPos.y, aGridPos.z, aType != VoxelType_EMPTY);
  }
  inline void SetVoxel(VoxelIndex aIndex, VoxelType aType) {
    xyzVoxels[aIndex] = aType;

    // also set cache here, decompose coordinates
    glm::vec3 co = getVoxelGridPosition(aIndex);
    UpdateVoxelCache(co.x, co.y, co.z, aType); 
    UpdateBitsets(co.x, co.y, co.z, aType != VoxelType_EMPTY);
  }

  // GETTERS
  inline Voxel &GetVoxel(const glm::ivec3 &aGridPos) {
    VoxelIndex index = aGridPos.x + aGridPos.y * CHUNK_SIZE +
                       aGridPos.z * CHUNK_SIZE * CHUNK_SIZE;
    return xyzVoxels[index];
  }
  inline Voxel &GetVoxel(VoxelIndex aIndex) { return xyzVoxels[aIndex]; }

  inline void SwapVoxels(VoxelIndex aIndex1, VoxelIndex aIndex2) {
    std::swap(xyzVoxels[aIndex1], xyzVoxels[aIndex2]);
    glm::vec3 p1 = getVoxelGridPosition(aIndex1);
    glm::vec3 p2 = getVoxelGridPosition(aIndex2);
    UpdateVoxelCache(p1.x, p1.y, p1.z, (VoxelType)xyzVoxels[aIndex1]);
    UpdateVoxelCache(p2.x, p2.y, p2.z, (VoxelType)xyzVoxels[aIndex2]);
    UpdateBitsets(p1.x, p1.y, p1.z, xyzVoxels[aIndex1] != VoxelType_EMPTY);
    UpdateBitsets(p2.x, p2.y, p2.z, xyzVoxels[aIndex2] != VoxelType_EMPTY);
  }


  // Returns the side length of the voxel chunk
  inline VoxelIndex GetSize() const { return CHUNK_SIZE; }

  // we'll work on this
  std::vector<glm::ivec3> getNonEmpty();

  // I know you. I don't like you. -- Joe Biden
  template <typename Pred>
  void QueryVoxels(Pred aPredicate, std::vector<VoxelIndex> &outIndices) {
    size_t size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    for (VoxelIndex i = 0; i < size; i++) {
      if (aPredicate(i, xyzVoxels[i])) {
        outIndices.push_back(i);
      }
    }
  }

  uint64_t CountNonEmptyVoxels() const;

  inline glm::mat4 GetTransform() const{
    constexpr glm::vec3 scale = glm::vec3(4.0f / CHUNK_SIZE);
    return glm::scale(glm::mat4(1.0), scale); 
  }

private:
  inline void UpdateBitsets(int aX, int aY, int aZ, bool isSolid) {
    if (isSolid) {
      zyOccupancy[aZ + aY * CHUNK_SIZE] |= VoxelBitset(1) << aX;
      xzOccupancy[aX + aZ * CHUNK_SIZE] |= VoxelBitset(1) << aY;
      xyOccupancy[aX + aY * CHUNK_SIZE] |= VoxelBitset(1) << aZ;
    } else {
      zyOccupancy[aZ + aY * CHUNK_SIZE] &= ~(VoxelBitset(1) << aX);
      xzOccupancy[aX + aZ * CHUNK_SIZE] &= ~(VoxelBitset(1) << aY);
      xyOccupancy[aX + aY * CHUNK_SIZE] &= ~(VoxelBitset(1) << aZ);
    }
  }

  inline void UpdateVoxelCache(int aX, int aY, int aZ, VoxelType aType) {
    int targetIndex = (int)aType - 1;
    int typeStride = CHUNK_SIZE * CHUNK_SIZE;

    // xy axis (bitset over z)
    int xyOffset = aX + aY * CHUNK_SIZE;
    for(int t=0; t<VOXEL_TYPES; t++)
      xyIsolatedVoxels[xyOffset + t * typeStride] &= ~(VoxelBitset(1) << aZ);

    // zy axis (bitset over x)
    int zyOffset = aZ + aY * CHUNK_SIZE;
    for(int t=0; t<VOXEL_TYPES; t++)
      zyIsolatedVoxels[zyOffset + t * typeStride] &= ~(VoxelBitset(1) << aX);

    // xz axis (bitset over y)
    int xzOffset = aX + aZ * CHUNK_SIZE;
    for(int t=0; t<VOXEL_TYPES; t++)
      xzIsolatedVoxels[xzOffset + t * typeStride] &= ~(VoxelBitset(1) << aY);

    if(aType == VoxelType_EMPTY) return;
    xyIsolatedVoxels[xyOffset + targetIndex * typeStride] |= (VoxelBitset(1) << aZ);
    zyIsolatedVoxels[zyOffset + targetIndex * typeStride] |= (VoxelBitset(1) << aX);
    xzIsolatedVoxels[xzOffset + targetIndex * typeStride] |= (VoxelBitset(1) << aY);
  }

public:
  // Store all voxel types in an array
  Voxel *xyzVoxels;

  // Store occupancy in bitsets all three
  // axis.

  VoxelBitset *zyOccupancy;
  VoxelBitset *xzOccupancy;
  VoxelBitset *xyOccupancy;

  // store caches for different types,
  // faster to iterate on specific blocks
  VoxelBitset *xyIsolatedVoxels;
  VoxelBitset *zyIsolatedVoxels;
  VoxelBitset *xzIsolatedVoxels;

  bool isDirty = false;
};
