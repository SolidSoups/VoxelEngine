#pragma once


#include <vector>

#include <glm/vec3.hpp>

#define CHUNK_SIZE 32
using voxel_index = uint16_t;
using Voxel = uint8_t;
enum VoxelType : uint8_t {
  VoxelType_EMPTY = 0,
  VoxelType_STONE = 1,
  VoxelType_SAND = 2,
  VoxelType_WATER = 3,

  VoxelType_PREVIEW,
  VoxelType_MAX
};
inline std::vector<const char*> ourVoxelNames = {
  "EMPTY",
  "STONE",
  "SAND",
  "WATER"
};
inline std::vector<glm::vec3> ourVoxelColors = {
  {0.0f, 0.0f, 0.0f},       // EMPTY
  {0.53f, 0.55f, 0.55f},    // STONE
  {0.96f, 0.88f, 0.69f},    // SAND
  {0.227f, 0.247f, 0.788f}  // WATER
};


// helper functions

inline glm::ivec3 getVoxelGridPosition(voxel_index aIndex) { 
  return {
    aIndex % CHUNK_SIZE,
    (aIndex / CHUNK_SIZE) % CHUNK_SIZE,
    aIndex / (CHUNK_SIZE * CHUNK_SIZE)
  };
}

inline voxel_index getVoxelIndex(const glm::ivec3& aGridPosition) {
  return aGridPosition.x + aGridPosition.y * CHUNK_SIZE + aGridPosition.z * CHUNK_SIZE * CHUNK_SIZE; 
}
