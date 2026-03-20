#include "KeybindsEditor.h"

#include <imgui.h>

void KeybindsEditor::Draw(){
  ImGui::Begin("Controls");
  ImGui::Text("Viewport");
  ImGui::BulletText("Orbit -- Shift + RMB + Drag");
  ImGui::BulletText("Pan -- MMD + Drag");
  ImGui::BulletText("Zoom -- Scroll");
  ImGui::BulletText("Reset Camera -- R");
  ImGui::Separator();
  ImGui::Text("Application");
  ImGui::BulletText("Close Application -- ESC");
  ImGui::End();
}

