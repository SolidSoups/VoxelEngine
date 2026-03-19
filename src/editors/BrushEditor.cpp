#include "BrushEditor.h"

#include <print>
#include <imgui.h>
#include "ImGuiHelpers.h"
#include "../VoxelPainter.h"

void BrushEditor::Draw() {
  ImGui::Begin("Voxel Painter");

  // allow user to select voxel type
  const char *typeItems[] = {"EMPTY", "STONE", "SAND"};
  static int selectedTypeIdx = 0;
  if(ImGuiHelpers::DrawCombo("Voxel Type", typeItems, IM_ARRAYSIZE(typeItems), selectedTypeIdx)){
    // user selected a new voxel type
    switch(selectedTypeIdx){
      case 0:
        VoxelPainter::SetBrushColor(VoxelType::VoxelType_EMPTY);
        break;
      case 1:
        VoxelPainter::SetBrushColor(VoxelType::VoxelType_STONE);
        break;
      case 2:
        VoxelPainter::SetBrushColor(VoxelType::VoxelType_SAND);
        break;
    }
  }

  // allow user to select brush type
  const char *brushTypes[] = {"VOXEL", "RECTOID", "SPHERE"};
  static int selectedBrushIdx = 0;
  if(ImGuiHelpers::DrawCombo("Brush Type", brushTypes, IM_ARRAYSIZE(brushTypes), selectedBrushIdx)){
    // user selected a new voxel type
    switch(selectedBrushIdx){
      case 0:
        VoxelPainter::SetBrushType(BrushType_VOXEL);
        break;
      case 1:
        VoxelPainter::SetBrushType(BrushType_RECTOID);
        break;
      case 2:
        VoxelPainter::SetBrushType(BrushType_SPHERE);
        break;
    }
  }

  ImGui::End();
}
