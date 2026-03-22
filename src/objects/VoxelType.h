#pragma once


#define CHUNK_SIZE 32
using voxel_index = uint16_t;
using Voxel = uint8_t;
enum VoxelType : uint8_t {
  VoxelType_EMPTY = 0,
  VoxelType_STONE = 1,
  VoxelType_SAND = 2,

  VoxelType_PREVIEW,
  VoxelType_MAX
};
