#include "VoxelPainter.h"

#include <print>
#include <vector>
#include <algorithm>

#include "objects/VoxelChunk.h"
#include "helpers/AABB.h"

VoxelType VoxelPainter::myBrushColor = VoxelType_EMPTY;
BrushType VoxelPainter::myBrushType = BrushType_VOXEL;
VoxelChunk *VoxelPainter::myCurrentChunk = nullptr;

PainterState& PainterState::Get(){
  static PainterState state;
  return state;
}

void VoxelPainter::SetBrushColor(VoxelType aVoxelType) {
  VoxelPainter::myBrushColor = aVoxelType;
}

void VoxelPainter::SetBrushType(BrushType aBrushType) {
  VoxelPainter::myBrushType = aBrushType;
}
void VoxelPainter::SetCurrentChunk(VoxelChunk *aChunk) {
  VoxelPainter::myCurrentChunk = aChunk;
}

void VoxelPainter::EditorPaint(const glm::ivec3 &aGridPosA,
                               const glm::ivec3 &aGridPosB, int aRadius) {
  if (VoxelPainter::myCurrentChunk) {
    switch (VoxelPainter::myBrushType) {
    case BrushType_VOXEL:
      PaintVoxel(aGridPosA, *VoxelPainter::myCurrentChunk);
      break;
    case BrushType_RECTOID:
      PaintRect(aGridPosA, aGridPosB, *VoxelPainter::myCurrentChunk);
      break;
    case BrushType_SPHERE:
      PaintSphere(aGridPosA, aRadius, *VoxelPainter::myCurrentChunk);
      break;
    default:
      std::println("Brush type {0} not implemented yet",
                   (int)VoxelPainter::myBrushType);
      break;
    }
  }
}

// TODO: cleanup this ridiculous template shit
void VoxelPainter::PaintSphere(const glm::ivec3 &aGridCoordinate, int aRadius,
                               VoxelChunk &aChunk) {
  glm::vec3 gridPosition{aGridCoordinate};

  // query for voxels in the area
  std::vector<VoxelIndex> result;
  aChunk.QueryVoxels(
      [gridPosition, aRadius](VoxelIndex i, Voxel v) {
        glm::vec3 voxelPos{getVoxelGridPosition(i)};
        if (glm::length(voxelPos - gridPosition) <= aRadius) {
          return true;
        }
        return false;
      },
      result);

  // set those voxels
  for (VoxelIndex &i : result) {
    aChunk.SetVoxel(i, VoxelPainter::myBrushColor);
  }

  aChunk.isDirty = true;
}

// terribly inneficient
void VoxelPainter::PaintRect(const glm::ivec3 &aGridPosA,
                             const glm::ivec3 &aGridPosB, VoxelChunk &aChunk) {
  AABB aabb = createAABB(aGridPosA, aGridPosB);
  for (int x = aabb.min.x; x <= aabb.max.x; x++)
    for (int y = aabb.min.y; y <= aabb.max.y; y++)
      for (int z = aabb.min.z; z <= aabb.max.z; z++) {
        aChunk.SetVoxel({x, y, z}, VoxelPainter::myBrushColor);
      }
  aChunk.isDirty = true;
}

void VoxelPainter::PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk &aChunk) {
  aChunk.SetVoxel(aGridPos, VoxelPainter::myBrushColor);
  aChunk.isDirty = true;
}
glm::ivec3 VoxelPainter::DDARaycastGetPosition(const glm::vec3 &aRayOrigin, const glm::vec3& aRayDirection, const VoxelChunk& aChunk){
  constexpr float stepSize = 4.0f / CHUNK_SIZE; 
  constexpr int MAX_STEPS = CHUNK_SIZE*CHUNK_SIZE;

  // Transform ray into grid space where 1 unit is 1 voxel
  glm::vec3 gridOrigin = aRayOrigin / stepSize;
  gridOrigin += 0.5f; // offset to match centered voxel rendering
  glm::vec3 rayDir = glm::normalize(aRayDirection);

  // voxel were currently in
  int cellX = (int)floor(gridOrigin.x);
  int cellY = (int)floor(gridOrigin.y);
  int cellZ = (int)floor(gridOrigin.z);

  // if origin is outside of the chunk, we need
  // to advance the ray to the entry point
  if(cellX < 0 or cellX >= CHUNK_SIZE or
     cellY < 0 or cellY >= CHUNK_SIZE or
     cellZ < 0 or cellZ >= CHUNK_SIZE){
    // Slab intersection against the chunk AABB in grid space
    float tMin = 0.0f;
    float tMax = FLT_MAX;

    for(int axis=0; axis<3; axis++){
      float invDir = 1.f / rayDir[axis];
      float t0 = (0.f - gridOrigin[axis]) * invDir;
      float t1 = ((float)CHUNK_SIZE - gridOrigin[axis]) * invDir;
      if(invDir < 0.f) std::swap(t0, t1);
      tMin = std::max(tMin, t0);
      tMax = std::min(tMax, t1);
    }

    // ray missed chunk entirely
    if(tMin > tMax) return glm::ivec3(-1);

    // advance origin to entry point
    gridOrigin = gridOrigin + rayDir * (tMin + 0.001f); 
    cellX = (int)floor(gridOrigin.x);
    cellY = (int)floor(gridOrigin.y);
    cellZ = (int)floor(gridOrigin.z);

    // clamp to handle floating point errors
    cellX = glm::clamp(cellX, 0, CHUNK_SIZE - 1);
    cellY = glm::clamp(cellY, 0, CHUNK_SIZE - 1);
    cellZ = glm::clamp(cellZ, 0, CHUNK_SIZE - 1);
  }


  // Directions to step along in each axis
  int stepDirX = (rayDir.x >= 0) ? 1 : -1;
  int stepDirY = (rayDir.y >= 0) ? 1 : -1;
  int stepDirZ = (rayDir.z >= 0) ? 1 : -1;

  // the amount t is incremented for one unit step along an axis
  float tPerXStep = fabs(1.f / rayDir.x);
  float tPerYStep = fabs(1.f / rayDir.y);
  float tPerZStep = fabs(1.f / rayDir.z);

  // distance in t from ray origin to first cell boundary on each axis
  float tMX = (stepDirX > 0 ? (cellX + 1 - gridOrigin.x) : (gridOrigin.x - cellX)) * tPerXStep;
  float tMY = (stepDirY > 0 ? (cellY + 1 - gridOrigin.y) : (gridOrigin.y - cellY)) * tPerYStep;
  float tMZ = (stepDirZ > 0 ? (cellZ + 1 - gridOrigin.z) : (gridOrigin.z - cellZ)) * tPerZStep;

  int prevX = cellX, prevY = cellY, prevZ = cellZ;

  for(int i=0; i<MAX_STEPS; i++){
    // out of bounds
    if(cellX < 0 or cellX >= CHUNK_SIZE or
       cellY < 0 or cellY >= CHUNK_SIZE or
       cellZ < 0 or cellZ >= CHUNK_SIZE) break;

    if((aChunk.zyOccupancy[cellZ + cellY * CHUNK_SIZE] >> cellX) & 1)
      return glm::ivec3(prevX, prevY, prevZ); // placement pos

    prevX = cellX; prevY = cellY; prevZ = cellZ;

    if(tMX < tMY and tMX < tMZ) { cellX += stepDirX; tMX += tPerXStep; }
    else if (tMY < tMZ)         { cellY += stepDirY; tMY += tPerYStep; }
    else                        {cellZ += stepDirZ; tMZ += tPerZStep; }
  }
  return glm::ivec3(-1); // miss
}
