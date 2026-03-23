#include "VoxelChunk.h"

// CONSTRUCTOR/DESTRUCTOR
VoxelChunk::VoxelChunk() {
  voxels = new Voxel[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
  memset(voxels, VoxelType::VoxelType_EMPTY,
         sizeof(Voxel) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
}
VoxelChunk::~VoxelChunk() { delete[] voxels; }

// MOVE ASSIGNMENT AND OPERATOR
VoxelChunk::VoxelChunk(VoxelChunk &&other) noexcept : voxels(other.voxels) {
  other.voxels = nullptr;
}
VoxelChunk &VoxelChunk::operator=(VoxelChunk &&other) {
  if (this != &other) {
    delete[] voxels;
    voxels = other.voxels;
    other.voxels = nullptr;
  }
  return *this;
}

// SOME WEIRDO FUNCTIONS
std::vector<glm::ivec3> VoxelChunk::getNonEmpty() {
  std::vector<glm::ivec3> result;
  for (int z = 0; z < CHUNK_SIZE; z++)
    for (int y = 0; y < CHUNK_SIZE; y++)
      for (int x = 0; x < CHUNK_SIZE; x++) {
        if (GetVoxel({x, y, z}) != VoxelType_EMPTY)
          result.push_back({x, y, z});
      }
  return result;
}
