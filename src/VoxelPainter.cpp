#include "VoxelPainter.h"

#include <print>
#include <vector>

#include "objects/VoxelChunk.h"
#include "helpers/AABB.h"

VoxelType VoxelPainter::myBrushColor = VoxelType_EMPTY;
BrushType VoxelPainter::myBrushType = BrushType_VOXEL;
VoxelChunk *VoxelPainter::myCurrentChunk = nullptr;

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
  std::vector<voxel_index> result;
  aChunk.QueryVoxels(
      [gridPosition, aRadius](voxel_index i, Voxel v) {
        glm::vec3 voxelPos{getVoxelGridPosition(i)};
        if (glm::length(voxelPos - gridPosition) <= aRadius) {
          return true;
        }
        return false;
      },
      result);

  // set those voxels
  for (voxel_index &i : result) {
    aChunk.SetVoxel(i, VoxelPainter::myBrushColor);
  }
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
}

void VoxelPainter::PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk &aChunk) {
  aChunk.SetVoxel(aGridPos, VoxelPainter::myBrushColor);
}
