#include "editors/BrushEditor.h"

#include <glm/glm.hpp>
#include <imgui.h>
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


  ImGui::End();

}

void BrushEditor::DrawPalette(){
  // func for image button with selectable
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

  // calc width and height so imgui doesn't steal all space
  float totalWidth = BUTTON_COUNT * PALETTE_BUTTON_SIZE.x +
                     (BUTTON_COUNT - 1) * ImGui::GetStyle().ItemSpacing.x +
                     ImGui::GetStyle().FramePadding.x * 2 * BUTTON_COUNT;
  float totalHeight = PALETTE_BUTTON_SIZE.y + ImGui::GetStyle().FramePadding.y * 2;


  // draw all voxel type options
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
  // function for brushes
  auto ImagePaletteButton = [&](const char* id, Texture&tex, BrushType type){
    bool selected = (myBrushType == type);
    if(selected)
      ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if(ImGui::ImageButton(id, (ImTextureRef)(intptr_t)tex.id, PALETTE_BUTTON_SIZE)){
      myBrushType = selected ? BrushType_VOXEL : type;
      auto& painterState = PainterState::Get();
      painterState.brush = (BrushType)myBrushType;
    }
    if(selected)
      ImGui::PopStyleColor();
  };

  // calc width and height
  float totalWidth = BRUSH_COUNT * PALETTE_BUTTON_SIZE.x +
                     (BRUSH_COUNT - 1) * ImGui::GetStyle().ItemSpacing.x +
                     ImGui::GetStyle().FramePadding.x * 2 * BRUSH_COUNT;
  float totalHeight = PALETTE_BUTTON_SIZE.y + ImGui::GetStyle().FramePadding.y * 2;

  // draw brushes
  ImGui::Text("Brushes");
  ImGui::BeginChild("##BrushPalette", ImVec2(totalWidth, totalHeight));
  ImagePaletteButton("##SphereBrush", *myPenTexture, BrushType_SPHERE);
  ImGui::EndChild();

  ImGui::Dummy(ImVec2(0, 3));

  // draw radius
  auto& paintState = PainterState::Get();
  int rad = paintState.radius;
  if(myBrushType == BrushType_SPHERE){
    if(ImGui::SliderInt("Radius", &rad, 1, (CHUNK_SIZE / 2) - 1)){
      paintState.radius = rad;
    }
  }
}

