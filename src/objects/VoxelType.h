#pragma once

using VoxelIndex = uint16_t;
using Voxel = uint8_t;
enum VoxelType : uint8_t {
  EMPTY = 0,
  STONE = 1,
  SAND = 2,

  MAX
};
