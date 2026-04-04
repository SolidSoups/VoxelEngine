#pragma once

#include <memory>

#include "Editor.h"
#include "voxel/VoxelType.h"
#include "voxel/PaintType.h"

struct Texture;

class BrushEditor : public Editor{
public:
  BrushEditor();
  ~BrushEditor();
  void Draw() override; 

private:
  void DrawVoxelType();
  void DrawBrushType();
  void DrawIVec3(const char* aName, const char* aID, glm::ivec3& aVec3);
  void DrawPalette();
  void DrawBrushes();
  VoxelType mySelectedVoxelType = VoxelType_EMPTY;
  BrushType myBrushType = BrushType_VOXEL; 

private: // textures
  std::unique_ptr<Texture> myPenTexture;
  std::unique_ptr<Texture> myEmptyVoxelTexture;
  std::unique_ptr<Texture> myStoneVoxelTexture;
  std::unique_ptr<Texture> mySandVoxelTexture;
  std::unique_ptr<Texture> myWaterVoxelTexture;
};
