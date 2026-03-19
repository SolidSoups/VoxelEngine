#include "VoxelPainter.h"

#include <vector>

#include "objects/VoxelChunk.h"
#include "helpers/AABB.h"

VoxelType VoxelPainter::myBrushColor = STONE;

void VoxelPainter::SetBrushColor(VoxelType aVoxelType) {
  VoxelPainter::myBrushColor = aVoxelType;
}

void VoxelPainter::PaintSphere(const glm::ivec3 &aGridCoordinate, int aRadius,
                               VoxelChunk &aChunk) {
  glm::vec3 gridPosition{aGridCoordinate};

  // query for voxels in the area
  std::vector<VoxelIndex> result;
  aChunk.QueryVoxels(
      [gridPosition, aRadius](VoxelIndex i, Voxel v) {
        glm::vec3 voxelPos{getVGridPos(i)};
        if (glm::length(voxelPos - gridPosition) <= aRadius) {
          return true;
        }
        return false;
      },
      result);

  // set those voxels
  for (VoxelIndex &i : result) {
    aChunk[i] = VoxelPainter::myBrushColor;
  }
}

// terribly inneficient
void VoxelPainter::PaintRect(const glm::ivec3 &aGridPosA,
                             const glm::ivec3 &aGridPosB, VoxelChunk &aChunk) {
  AABB aabb = createAABB(aGridPosA, aGridPosB);
  for (int x = aabb.min.x; x <= aabb.max.x; x++)
    for (int y = aabb.min.y; y <= aabb.max.y; y++)
      for (int z = aabb.min.z; z <= aabb.max.z; z++) {
        aChunk[x, y, z] = VoxelPainter::myBrushColor;
      }
}

void VoxelPainter::PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk& aChunk){
  aChunk[aGridPos.x, aGridPos.y, aGridPos.z] = VoxelPainter::myBrushColor;
}
