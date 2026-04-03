#include "editors/EditorIO.h"
#include "imgui.h"


EditorIO& EditorIO::Get(){
  static EditorIO io;
  return io;
}

glm::ivec2 EditorIO::GetViewportMousePosition(){
  auto& io = EditorIO::Get();
  ImVec2 imMousePos = ImGui::GetMousePos();
  return glm::ivec2(imMousePos.x, imMousePos.y) - io.ViewportMin;
}
