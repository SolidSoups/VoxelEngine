#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void EditorLayer::Initialize(GLFWwindow *aWindow) {
  // Set dear imgui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // setup platform/renderer backends
  ImGui_ImplGlfw_InitForOpenGL(aWindow, true);
  ImGui_ImplOpenGL3_Init();
}

void EditorLayer::BeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow(); // Show demo window! :)
}

void EditorLayer::EndFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::Shutdown(){
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
