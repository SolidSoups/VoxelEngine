#pragma once

#include <glm/vec2.hpp>
#include "../voxel/VoxelType.h"

struct VoxelChunk;

struct VoxelContext
{
    VoxelChunk       &dst;
    const VoxelChunk &src;
    const Voxel       voxel;
    // defined as x + y * width + z * width^2
    const size_t     index;
    const glm::ivec3 gridPos;
    const uint8_t* xzSurfaceHeightMap;
    const glm::vec2* xzSurfaceSlopeMap;
};

bool SinkThroughWater(const VoxelContext &ctx);
bool FallDown(const VoxelContext &ctx);
bool FallDiagonally(const VoxelContext &ctx);
bool SpreadHorizontally(const VoxelContext &ctx);
bool SurfaceWaterSpread(const VoxelContext &ctx);

bool FindHorizontalTarget(const VoxelContext &ctx, VoxelIndex &outIndex);
bool FindDiagonalTarget(const VoxelContext &ctx, VoxelIndex &outIndex);

int CountHorizontalNeighbours(const VoxelContext &ctx);
// Compute the center of mass for a horizontal xz plane with a given radius
glm::vec2 CalcCenterOfMass(const VoxelContext &ctx, int aRadius=2);
glm::vec2 CalcSlopeDirection(const VoxelContext &ctx, int aRadius=2);

bool PickTargetFromDir(const VoxelContext& ctx, const glm::vec2& aDir, size_t &outCandidate);
