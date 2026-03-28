#include "VoxelChunk.h"
#include "VoxelType.h"

// CONSTRUCTOR/DESTRUCTOR
VoxelChunk::VoxelChunk() {
  // initialize voxels
  xyzVoxels = new Voxel[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
  memset(xyzVoxels, 0, sizeof(Voxel) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

  // initialize bitsets
  size_t size = CHUNK_SIZE * CHUNK_SIZE;
  zyOccupancy = new VoxelBitset[size];
  xzOccupancy = new VoxelBitset[size];
  xyOccupancy = new VoxelBitset[size];
  memset(zyOccupancy, 0, sizeof(VoxelBitset) * size);
  memset(xzOccupancy, 0, sizeof(VoxelBitset) * size);
  memset(xyOccupancy, 0, sizeof(VoxelBitset) * size);

  // Initialize isolated voxel views
  for (int i = 0; i < VOXEL_TYPES; i++) {
    xyIsolatedVoxels[i] = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE];
    memset(xyIsolatedVoxels[i], 0, sizeof(VoxelBitset) * CHUNK_SIZE * CHUNK_SIZE);
  }
}
VoxelChunk::~VoxelChunk() {
  delete[] xyzVoxels;
  delete[] xyOccupancy;
  delete[] xzOccupancy;
  delete[] zyOccupancy;
  for (int i = 0; i < VOXEL_TYPES; i++) {
    if (xyIsolatedVoxels[i])
      delete[] xyIsolatedVoxels[i];
  }
}

// MOVE ASSIGNMENT AND OPERATOR
VoxelChunk::VoxelChunk(VoxelChunk &&other) noexcept
    : xyzVoxels(other.xyzVoxels) {
  other.xyzVoxels = nullptr;
}
VoxelChunk &VoxelChunk::operator=(VoxelChunk &&other) {
  if (this != &other) {
    delete[] xyzVoxels;
    xyzVoxels = other.xyzVoxels;
    other.xyzVoxels = nullptr;
  }
  return *this;
}

// Optimized now
std::vector<glm::ivec3> VoxelChunk::getNonEmpty() {
  std::vector<glm::ivec3> result;
  size_t size = CHUNK_SIZE * CHUNK_SIZE;
  for (size_t i = 0; i < size; i++) {
    if (xzOccupancy[i])
      result.push_back(getVoxelGridPosition(i));
  }
  return result;
}

uint64_t VoxelChunk::CountNonEmptyVoxels() const {
  uint64_t count = 0;
  size_t size = CHUNK_SIZE * CHUNK_SIZE;
  for (size_t i = 0; i < size; i++) {
    count += std::popcount(xzOccupancy[i]);
  }
  return count;
}
