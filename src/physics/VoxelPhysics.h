#pragma once

#include "../voxel/VoxelType.h"

struct VoxelChunk;

struct VoxelContext {
  VoxelChunk &dst;
  const VoxelChunk& src;
  const size_t width;
  const Voxel voxel;
  const size_t index;
  const glm::ivec3 gridPos;
};

bool SinkThroughWater(const VoxelContext &ctx);
bool FallDown(const VoxelContext &ctx);
bool FallDiagonally(const VoxelContext &ctx);
bool SpreadHorizontally(const VoxelContext &ctx);

bool FindHorizontalTarget(const VoxelContext &ctx, VoxelIndex &outIndex);
bool FindDiagonalTarget(const VoxelContext &ctx,
                                  VoxelIndex &outIndex);
