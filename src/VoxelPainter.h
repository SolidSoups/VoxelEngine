#pragma once

#include <glm/glm.hpp>
#include "objects/VoxelType.h"
#include "objects/PaintType.h"

struct VoxelChunk;


class VoxelPainter {
public:
  // Draw a cube at a grid coordinate with a given radius
  static void SetBrushColor(VoxelType aVoxelType);
  static void SetBrushType(BrushType aBrushType);
  static void SetCurrentChunk(VoxelChunk* aChunk);
  static void PaintSphere(const glm::ivec3 &aGridCoordinate, int aRadius,
                          VoxelChunk &aScene);
  static void PaintRect(const glm::ivec3 &aGridPosA,
                        const glm::ivec3 &aGridPosB, VoxelChunk &aChunk);
  static void PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk &aChunk);
  static void EditorPaint(const glm::ivec3 &aGridPosA, const glm::ivec3& aGridPosB, int aRadius);

private:
  static VoxelType myBrushColor;
  static BrushType myBrushType;
  static VoxelChunk* myCurrentChunk;
};
