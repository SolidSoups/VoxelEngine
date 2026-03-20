#include "BrushEditor.h"

#include <print>

#include <glm/glm.hpp>
#include <imgui.h>
#include "ImGuiHelpers.h"
#include "../VoxelPainter.h"

void BrushEditor::Draw() {
  ImGui::Begin("Voxel Painter");

  DrawVoxelType();
  DrawBrushType();

  ImGui::Separator();

  static glm::ivec3 drawPositionA{0};
  ImGui::AlignTextToFramePadding();
  ImGui::Text("PositionA");
  ImGui::SameLine();
  ImGui::DragInt3("##PositionA", &drawPositionA.x, 1, 0, CHUNK_SIZE-1);

  ImGui::Dummy(ImVec2(0, 3));

  static glm::ivec3 drawPositionB{0};
  ImGui::AlignTextToFramePadding();
  ImGui::Text("PositionB");
  ImGui::SameLine();
  ImGui::DragInt3("##PositionB", &drawPositionB.x, 1, 0, CHUNK_SIZE-1);

  ImGui::Dummy(ImVec2(0, 3));

  static int radius = 0;
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Radius");
  ImGui::SameLine();
  ImGui::DragInt("##Radius", &radius, 1, 0);


  if(ImGui::Button("Paint")){
    VoxelPainter::SetBrushColor(mySelectedVoxelType);
    VoxelPainter::SetBrushType(mySelectedVoxelBrush);
    VoxelPainter::EditorPaint(drawPositionA, drawPositionB, radius);
  }
  ImGui::SameLine();
  if(ImGui::Button("Clear")){
    VoxelPainter::SetBrushColor(VoxelType_EMPTY);
    VoxelPainter::SetBrushType(BrushType_RECTOID);
    VoxelPainter::EditorPaint(glm::ivec3{0}, glm::ivec3{CHUNK_SIZE-1}, 0);
  }

  ImGui::End();
}


void BrushEditor::DrawVoxelType(){
  const char *typeItems[] = {"EMPTY", "STONE", "SAND"};
  static int selectedTypeIdx = (int)mySelectedVoxelType;
  if(ImGuiHelpers::DrawCombo("Voxel Type", typeItems, IM_ARRAYSIZE(typeItems), selectedTypeIdx)){
    mySelectedVoxelType = (VoxelType)selectedTypeIdx;
  }
}
void BrushEditor::DrawBrushType(){
  const char *brushTypes[] = {"VOXEL", "RECTOID", "SPHERE"};
  static int selectedBrushIdx = (int)mySelectedVoxelBrush;
  if(ImGuiHelpers::DrawCombo("Brush Type", brushTypes, IM_ARRAYSIZE(brushTypes), selectedBrushIdx)){
    mySelectedVoxelBrush = (BrushType)selectedBrushIdx;
  }
}
