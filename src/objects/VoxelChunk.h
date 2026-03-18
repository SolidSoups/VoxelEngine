#pragma once

#include <vector>
#include <glm/glm.hpp>

#define CHUNK_SIZE 4

using Voxel = uint8_t;
enum VoxelType : uint8_t {
  EMPTY = 0,
  STONE = 1,
  SAND = 2,

  MAX
};

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
};
