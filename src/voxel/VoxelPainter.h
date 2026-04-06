#pragma once

#include <glm/glm.hpp>
#include "voxel/VoxelType.h"
#include "voxel/PaintType.h"

struct VoxelChunk;

struct PainterState {
  glm::vec3 cursorPosition{0};
  int radius = 1;
  BrushType brush = BrushType_SPHERE;
  VoxelType color = VoxelType_EMPTY;
  bool drawCursor = false;

public:
  static PainterState &Get();
};

class VoxelPainter {
public:
  // Draw a cube at a grid coordinate with a given radius
  static void SetBrushColor(VoxelType aVoxelType);
  static void SetBrushType(BrushType aBrushType);
  static void SetCurrentChunk(VoxelChunk *aChunk);
  static void PaintSphere(const glm::ivec3 &aGridCoordinate, int aRadius,
                          VoxelChunk &aScene);
  static void PaintRect(const glm::ivec3 &aGridPosA,
                        const glm::ivec3 &aGridPosB, VoxelChunk &aChunk);
  static void PaintVoxel(const glm::ivec3 &aGridPos, VoxelChunk &aChunk);
  static void EditorPaint(const glm::ivec3 &aGridPosA,
                          const glm::ivec3 &aGridPosB, int aRadius);
  static glm::ivec3 DDARaycastGetPosition(const glm::vec3 &startPos,
                                          const glm::vec3 &direction,
                                          const VoxelChunk &aChunk);

private:
  static VoxelType myBrushColor;
  static BrushType myBrushType;
  static VoxelChunk *myCurrentChunk;
};
