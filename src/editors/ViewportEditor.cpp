#include "ViewportEditor.h"
#include <imgui.h>
#include "../Renderer.h"
#include "../objects/Framebuffer.h"
#include "../ApplicationLayer.h"

void ViewportEditor::Draw() {

  ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoInputs); 
  glm::vec2 size = ApplicationLayer::GetWindowSize();
  ImGui::Image((ImTextureRef)(intptr_t)Renderer::frameBuffer.texture, ImVec2(size.x, size.y), ImVec2(0, 1), ImVec2(1,0));
  ImGui::End();
}
