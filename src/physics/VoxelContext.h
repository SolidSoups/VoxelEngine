#pragma once

#include "../voxel/VoxelType.h"
#include "../voxel/VoxelChunk.h"

enum class VoxelAction : uint8_t
{
    None           = 0,
    FallDown       = 1,
    FallDiagonally = 2,
    SpreadH        = 3,
    SurfaceSpread  = 4,
    SinkThruWater  = 5
};

struct VoxelContext
{
    VoxelChunk       &dst;
    const VoxelChunk &src;
    const Voxel       voxel;

    // defined as x + y * width + z * width^2
    const size_t     index;
    const glm::ivec3 gridPos;
    const uint8_t   *xzSurfaceHeightMap;
    const glm::vec2 *xzSurfaceSlopeMap;
    const glm::vec2 *xzBFSSlopeMap;
    const glm::vec2 *xzWaveVelocity;
    uint8_t         *actionMap;
};
