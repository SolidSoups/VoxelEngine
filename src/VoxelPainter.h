#pragma once

#include <glm/glm.hpp>
#include "objects/VoxelType.h"

struct VoxelChunk;

enum BrushType : uint8_t {
  BrushType_VOXEL = 0,
  BrushType_RECTOID = 1,
  BrushType_SPHERE = 2,

  BrushType_MAX
};

class VoxelPainter {
public:
  // Draw a cube at a grid coordinate with a given radius
  static void SetBrushColor(VoxelType aVoxelType);
  static void SetBrushType(BrushType aBrushType);
  static void PaintSphere(const glm::ivec3 &aGridCoordinate, int aRadius,
                          VoxelChunk &aScene);
  static void PaintRect(const glm::ivec3 &aGridPosA,
                        const glm::ivec3 &aGridPosB, VoxelChunk &aChunk);
  static void PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk &aChunk);

private:
  static VoxelType myBrushColor;
  static BrushType myBrushType;
};
