#include "KeybindsEditor.h"

#include <imgui.h>

void KeybindsEditor::Draw(){
  ImGui::Begin("Controls");
  ImGui::Text("Viewport (Mouse)");
  ImGui::BulletText("Orbit -- Shift + RMB + Drag");
  ImGui::BulletText("Pan -- MMB + Drag");
  ImGui::BulletText("Zoom -- Scroll");
  ImGui::Separator();
  ImGui::Text("Viewport (Touchpad)");
  ImGui::BulletText("Orbit -- Alt + Mouse Move");
  ImGui::BulletText("Pan -- Shift + Mouse Move");
  ImGui::BulletText("Zoom -- Scroll / Ctrl + LMB + Drag");
  ImGui::Separator();
  ImGui::BulletText("Reset Camera -- R");
  ImGui::Separator();
  ImGui::Text("Application");
  ImGui::BulletText("Close Application -- ESC");
  ImGui::End();
}

