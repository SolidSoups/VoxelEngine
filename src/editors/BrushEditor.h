#pragma once

#include "../objects/VoxelType.h"
#include "../objects/PaintType.h"

class BrushEditor {
public:
  void Draw();

private:
  void DrawVoxelType();
  void DrawBrushType();
  VoxelType mySelectedVoxelType = VoxelType_EMPTY;
  BrushType mySelectedVoxelBrush = BrushType_VOXEL; 
};
