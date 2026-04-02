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

  static glm::ivec3 drawPositionA{32};
  static glm::ivec3 drawPositionB{0};
  static int radius = 10;

  ImGui::Dummy(ImVec2(0, 5));
  if(myBrushType == BrushType_VOXEL){
    DrawIVec3("Position", "##PositionA", drawPositionA);
  }
  else if(myBrushType == BrushType_RECTOID){
    DrawIVec3("Position A", "##PositionA", drawPositionA);
    ImGui::Dummy(ImVec2(0, 3));
    DrawIVec3("Position B", "##PositionB", drawPositionB);
  }
  else if(myBrushType == BrushType_SPHERE){
    DrawIVec3("Center", "##PositionA", drawPositionA);
    ImGui::Dummy(ImVec2(0, 3));
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Radius");
    ImGui::SameLine();
    ImGui::DragInt("##Radius", &radius, 1, 0);
  }

  ImGui::Dummy(ImVec2(0, 5));

  if(ImGui::Button("Paint")){
    VoxelPainter::SetBrushColor(mySelectedVoxelType);
    VoxelPainter::SetBrushType(myBrushType);
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
  static int selectedTypeIdx = (int)mySelectedVoxelType;
  if(ImGuiHelpers::DrawCombo("Voxel Type", ourVoxelNames.data(), ourVoxelNames.size(), selectedTypeIdx)){
    mySelectedVoxelType = (VoxelType)selectedTypeIdx;
  }
}
void BrushEditor::DrawBrushType(){
  const char *brushTypes[] = {"VOXEL", "RECTOID", "SPHERE"};
  static int selectedBrushIdx = (int)myBrushType;
  if(ImGuiHelpers::DrawCombo("Brush Type", brushTypes, IM_ARRAYSIZE(brushTypes), selectedBrushIdx)){
    myBrushType = (BrushType)selectedBrushIdx;
    auto& pState = PainterState::Get();
    pState.brush = (BrushType)selectedBrushIdx;
  }
}



void BrushEditor::DrawIVec3(const char* aName, const char* aID, glm::ivec3& aVec3){
  ImGui::AlignTextToFramePadding();
  ImGui::Text(aName);
  ImGui::SameLine();
  ImGui::DragInt3(aID, &aVec3.x, 1, 0, CHUNK_SIZE-1);
}
