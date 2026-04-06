#include "VoxelPhysics.h"
#include "../voxel/VoxelChunk.h"

bool SinkThroughWater(const VoxelContext &ctx) {
  if (ctx.gridPos.y <= 0)
    return false;

  // only move down if water under
  size_t indexUnder = ctx.index - ctx.width;
  if (ctx.src[indexUnder] != VoxelType_WATER)
    return false;

  // swap them in the dst
  ctx.dst[ctx.index] = VoxelType_WATER;
  ctx.dst[indexUnder] = VoxelType_SAND;

  return true;
}

bool FallDown(const VoxelContext &ctx) {
  // get voxel position, skip bottom
  if (ctx.gridPos.y == 0)
    return false;

  // only move down on empty
  VoxelIndex indexUnder = ctx.index - ctx.width;
  if (ctx.dst[indexUnder] != VoxelType_EMPTY)
    return false;

  // swap this voxel with the one under in the dst
  ctx.dst[ctx.index] = ctx.dst[indexUnder];
  ctx.dst[indexUnder] = ctx.voxel;
  return true;
}

bool FallDiagonally(const VoxelContext &ctx) {
  // we are at the bottom, skip
  if (ctx.gridPos.y == 0)
    return false;

  VoxelIndex chosen;
  if (!FindDiagonalTarget(ctx, chosen))
    return false;

  // swap
  ctx.dst[ctx.index] = ctx.dst[chosen]; 
  ctx.dst[chosen] = ctx.voxel;

  return true;
}

bool SpreadHorizontally(const VoxelContext &ctx) {
  // Add more spread if above is water ???
  bool aboveIsWater = ctx.gridPos.y < ctx.width - 1 &&
                      ctx.dst[ctx.index + ctx.width] == VoxelType_WATER;
  bool weAreWater = ctx.voxel == VoxelType_WATER;
  int spreadChance = aboveIsWater and weAreWater ? 1 : 3;
  if (rand() % spreadChance != 0)
    return false;

  // nothing below, keep falling. but only if we can still fall there
  if (ctx.gridPos.y > 0 and
      ctx.dst[ctx.index - ctx.width] == VoxelType_EMPTY
  )
    return false;

  VoxelIndex chosen;
  if (!FindHorizontalTarget(ctx, chosen)) {
    return false;
  }
  ctx.dst[ctx.index] = ctx.dst[chosen];
  ctx.dst[chosen] = ctx.voxel;
  return true;
}

bool FindDiagonalTarget(const VoxelContext &ctx,
                                             VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  // cache index under and the size of a z slice
  const VoxelIndex indexUnder = ctx.index - ctx.width;
  const VoxelIndex zSliceSize = ctx.width * ctx.width;

  // check diagonal left
  if (ctx.gridPos.x > 0 && 
      ctx.dst[indexUnder - 1] == VoxelType_EMPTY){
    candidates[count++] = indexUnder - 1;
  }
  // check diagonal right
  if (ctx.gridPos.x < ctx.width - 1 &&
      ctx.dst[indexUnder + 1] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder + 1;
  }
  // check diagonal front
  if (ctx.gridPos.z < ctx.width - 1 &&
      ctx.dst[indexUnder + zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder + zSliceSize;
  }
  // check diagonal back
  if (ctx.gridPos.z > 0 &&
      ctx.dst[indexUnder - zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = indexUnder - zSliceSize;
  }

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random diagonal candidate with this voxel
  outIndex = candidates[rand() % count];
  return true;
}

bool FindHorizontalTarget(const VoxelContext &ctx,
                                               VoxelIndex &outIndex) {
  VoxelIndex candidates[4];
  size_t count = 0;

  const VoxelIndex zSliceSize = ctx.width * ctx.width;

  // Horizontally left
  if (ctx.gridPos.x > 0 and ctx.dst[ctx.index - 1] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index - 1;
  }
  // Horizontally right
  if (ctx.gridPos.x < ctx.width - 1 and
      ctx.dst[ctx.index + 1] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index + 1;
  }
  // Horizontally forward
  if (ctx.gridPos.z < ctx.width - 1 and
      ctx.dst[ctx.index + zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index + zSliceSize;
  }
  // Horizontally back
  if (ctx.gridPos.z > 0 and
      ctx.dst[ctx.index - zSliceSize] == VoxelType_EMPTY) {
    candidates[count++] = ctx.index - zSliceSize;
  }

  // no candidates, return
  if (count == 0)
    return false;

  // swap a random horizontal candidate with this voxel
  outIndex = candidates[rand() % count];
  return true;
}
