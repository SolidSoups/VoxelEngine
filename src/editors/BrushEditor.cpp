#include "editors/BrushEditor.h"

#include <print>

#include <glm/glm.hpp>
#include <imgui.h>
#include "editors/ImGuiHelpers.h"
#include "rendering/Texture.h"
#include "voxel/VoxelType.h"
#include "voxel/VoxelPainter.h"

#define PALETTE_BUTTON_SIZE ImVec2(32, 32)
#define BUTTON_COUNT 4
#define BRUSH_COUNT 1

BrushEditor::~BrushEditor() = default;

BrushEditor::BrushEditor(){
  myPenTexture = std::make_unique<Texture>("res/pen.png");
  myEmptyVoxelTexture = std::make_unique<Texture>("res/Voxel_None.png");
  myStoneVoxelTexture = std::make_unique<Texture>("res/Voxel_Stone.png");
  mySandVoxelTexture = std::make_unique<Texture>("res/Voxel_Sand.png");
  myWaterVoxelTexture = std::make_unique<Texture>("res/Voxel_Water.png");
}

void BrushEditor::Draw() {

  ImGui::Begin("Voxel Painter");

  DrawPalette();
  ImGui::Dummy(ImVec2(0, 5));
  DrawBrushes();
  ImGui::Dummy(ImVec2(0, 5));


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
    if(ImGui::DragInt("##Radius", &radius, 1, 0, CHUNK_SIZE / 2 - 1)){
      auto& painterState = PainterState::Get();
      painterState.radius = radius;
    }
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

void BrushEditor::DrawPalette(){
  auto ImagePaletteButton = [&](const char* id, Texture&tex, VoxelType type){
    bool selected = (mySelectedVoxelType == type);
    if(selected)
      ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if(ImGui::ImageButton(id, (ImTextureRef)(intptr_t)tex.id, PALETTE_BUTTON_SIZE)){
      mySelectedVoxelType = type;
      auto& painterState = PainterState::Get();
      painterState.color = (VoxelType)mySelectedVoxelType;
    }
    if(selected)
      ImGui::PopStyleColor();
  };
  float totalWidth = BUTTON_COUNT * PALETTE_BUTTON_SIZE.x +
                     (BUTTON_COUNT - 1) * ImGui::GetStyle().ItemSpacing.x +
                     ImGui::GetStyle().FramePadding.x * 2 * BUTTON_COUNT;
  float totalHeight = PALETTE_BUTTON_SIZE.y + ImGui::GetStyle().FramePadding.y * 2;


  ImGui::Text("Voxel Palette");
  ImGui::BeginChild("##TypePalette", ImVec2(totalWidth, totalHeight));
  ImagePaletteButton("##EmptyVoxel", *myEmptyVoxelTexture, VoxelType_EMPTY);
  ImGui::SameLine();
  ImagePaletteButton("##StoneVoxel", *myStoneVoxelTexture, VoxelType_STONE);
  ImGui::SameLine();
  ImagePaletteButton("##SandVoxel", *mySandVoxelTexture, VoxelType_SAND);
  ImGui::SameLine();
  ImagePaletteButton("##WaterVoxel", *myWaterVoxelTexture, VoxelType_WATER);
  ImGui::EndChild();
}


void BrushEditor::DrawBrushes(){
  auto ImagePaletteButton = [&](const char* id, Texture&tex, BrushType type){
    bool selected = (myBrushType == type);
    if(selected)
      ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if(ImGui::ImageButton(id, (ImTextureRef)(intptr_t)tex.id, PALETTE_BUTTON_SIZE)){
      myBrushType = type;
      auto& painterState = PainterState::Get();
      painterState.brush = (BrushType)myBrushType;
    }
    if(selected)
      ImGui::PopStyleColor();
  };

  float totalWidth = BRUSH_COUNT * PALETTE_BUTTON_SIZE.x +
                     (BRUSH_COUNT - 1) * ImGui::GetStyle().ItemSpacing.x +
                     ImGui::GetStyle().FramePadding.x * 2 * BRUSH_COUNT;
  float totalHeight = PALETTE_BUTTON_SIZE.y + ImGui::GetStyle().FramePadding.y * 2;

  ImGui::Text("Brushes");
  ImGui::BeginChild("##BrushPalette", ImVec2(totalWidth, totalHeight));
  ImagePaletteButton("##SphereBrush", *myPenTexture, BrushType_SPHERE);
  ImGui::EndChild();
}

void BrushEditor::DrawVoxelType(){
  static int selectedTypeIdx = (int)mySelectedVoxelType;
  if(ImGuiHelpers::DrawCombo("Voxel Type", ourVoxelNames.data(), ourVoxelNames.size(), selectedTypeIdx)){
    mySelectedVoxelType = (VoxelType)selectedTypeIdx;
    auto& painterState = PainterState::Get();
    painterState.color = (VoxelType)selectedTypeIdx;
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
