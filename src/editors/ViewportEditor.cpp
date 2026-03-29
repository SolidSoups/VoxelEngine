#include "ViewportEditor.h"
#include <imgui.h>
#include "../Renderer.h"
#include "../objects/Framebuffer.h"
#include "../ApplicationLayer.h"

void ViewportEditor::Draw() {
  ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoInputs); 
  ImVec2 size = ImGui::GetContentRegionAvail();
  viewportSize = glm::ivec2(size.x, size.y);
  ImGui::Image((ImTextureRef)(intptr_t)Renderer::framebuffer.textures[0], size, ImVec2(0, 1), ImVec2(1,0));
  ImGui::End();
}
