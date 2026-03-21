#pragma once

#include "../objects/VoxelType.h"
#include "../objects/PaintType.h"

class BrushEditor{
public:
  void Draw(); 

private:
  void DrawVoxelType();
  void DrawBrushType();
  void DrawIVec3(const char* aName, const char* aID, glm::ivec3& aVec3);
  VoxelType mySelectedVoxelType = VoxelType_EMPTY;
  BrushType myBrushType = BrushType_VOXEL; 
};
