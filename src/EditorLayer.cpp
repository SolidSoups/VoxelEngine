#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "editors/Editor.h"
#include "editors/PhysicsEditor.h"
#include "editors/KeybindsEditor.h"
#include "editors/BrushEditor.h"

std::unique_ptr<KeybindsEditor> EditorLayer::myKeybindsEditor;
std::unique_ptr<BrushEditor> EditorLayer::myBrushEditor;
std::vector<std::unique_ptr<Editor>> EditorLayer::myEditors;

void EditorLayer::Initialize(GLFWwindow *aWindow) {
  // Set dear imgui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // setup platform/renderer backends
  ImGui_ImplGlfw_InitForOpenGL(aWindow, true);
  ImGui_ImplOpenGL3_Init();

  // initialize editors
  myKeybindsEditor = std::make_unique<KeybindsEditor>();
  myBrushEditor = std::make_unique<BrushEditor>();
}

void EditorLayer::BeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow(); // Show demo window! :)

  myKeybindsEditor->Draw();
  myBrushEditor->Draw();
  for(auto& editor : EditorLayer::myEditors){
    editor->Draw();
  }
}

void EditorLayer::AddRuntimeEditor(PhysicsRuntime* aPhysicsRuntime){
  EditorLayer::myEditors.push_back(
    std::make_unique<PhysicsEditor>(aPhysicsRuntime)
  );
}

void EditorLayer::EndFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::Shutdown() {
  myKeybindsEditor.reset();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
