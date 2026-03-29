#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "editors/Editor.h"
#include "editors/PhysicsEditor.h"
#include "editors/KeybindsEditor.h"
#include "editors/BrushEditor.h"
#include "editors/ViewportEditor.h"
#include "imgui_internal.h"

std::unique_ptr<KeybindsEditor> EditorLayer::myKeybindsEditor;
std::unique_ptr<BrushEditor> EditorLayer::myBrushEditor;
std::vector<std::unique_ptr<Editor>> EditorLayer::myEditors;
std::unique_ptr<ViewportEditor> EditorLayer::myViewportEditor = nullptr;
unsigned int EditorLayer::myMainDockspace = 0;

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
  myViewportEditor = std::make_unique<ViewportEditor>();
}

void EditorLayer::BeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  myMainDockspace = ImGui::DockSpaceOverViewport();
  static bool firstFrame = true;
  if(firstFrame){
    ImGui::DockBuilderDockWindow("Viewport", myMainDockspace);
    firstFrame = false;
  }


  myKeybindsEditor->Draw();
  myBrushEditor->Draw();
  myViewportEditor->Draw();
  for(auto& editor : EditorLayer::myEditors){
    editor->Draw();
  }
}

glm::ivec2 EditorLayer::GetViewportSize(){
  return EditorLayer::myViewportEditor->viewportSize;
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
