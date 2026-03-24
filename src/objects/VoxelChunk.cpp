#include "VoxelChunk.h"
#include "VoxelType.h"

// CONSTRUCTOR/DESTRUCTOR
VoxelChunk::VoxelChunk() {
  // initialize voxels
  voxels = new Voxel[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
  memset(voxels, 0,
         sizeof(Voxel) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

  // initialize bitsets
  size_t size = CHUNK_SIZE * CHUNK_SIZE; 
  xRows = new VoxelBitset[size];
  yColumns = new VoxelBitset[size];
  zRows = new VoxelBitset[size];
  memset(xRows, 0, sizeof(VoxelBitset) * size);
  memset(yColumns, 0, sizeof(VoxelBitset) * size);
  memset(zRows, 0, sizeof(VoxelBitset) * size);
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

// Optimized now
std::vector<glm::ivec3> VoxelChunk::getNonEmpty() {
  std::vector<glm::ivec3> result;
  size_t size = CHUNK_SIZE * CHUNK_SIZE;
  for(size_t i=0; i<size; i++){
    if(yColumns[i])
      result.push_back(getVoxelGridPosition(i));
  } 
  return result;
}


uint64_t VoxelChunk::CountNonEmptyVoxels() const {
  uint64_t count = 0; 
  size_t size = CHUNK_SIZE * CHUNK_SIZE;
  for(size_t i=0; i<size; i++){
    count += std::popcount(yColumns[i]);
  }
  return count;
}
