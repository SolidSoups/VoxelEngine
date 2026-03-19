#include "KeybindsEditor.h"

#include <imgui.h>

void KeybindsEditor::Draw(){
  ImGui::Begin("Controls");
  ImGui::Text("Mouse");
  ImGui::BulletText("RMB + Drag - Orbit");
  ImGui::BulletText("MMB + Drag - Pan");
  ImGui::BulletText("Scroll - Zoom");
  ImGui::Separator();
  ImGui::Text("Keyboard");
  ImGui::BulletText("R - Reset Camera");
  ImGui::BulletText("ESC - Close Application");
  ImGui::End();
}

