#include "editors/ViewportEditor.h"
#include <imgui.h>
#include "rendering/Renderer.h"
#include "rendering/Framebuffer.h"
#include "app/ApplicationLayer.h"
#include "editors/EditorIO.h"

void ViewportEditor::Draw() {
  ImGui::Begin("Viewport", nullptr); 
  ImVec2 size = ImGui::GetContentRegionAvail();
  viewportSize = glm::ivec2(size.x, size.y);
  ImVec2 min = ImGui::GetCursorScreenPos();
  ImGui::Image((ImTextureRef)(intptr_t)Renderer::framebuffer.textures[0], size, ImVec2(0, 1), ImVec2(1,0));

  EditorIO& io = EditorIO::Get();
  io.ViewportIsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
  io.ViewportSize = viewportSize;
  io.ViewportMin = glm::ivec2{min.x, min.y};

  ImGui::End();
}
