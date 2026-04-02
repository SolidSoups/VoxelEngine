#pragma once
#include <glm/glm.hpp>

class EditorIO{
public:
  static EditorIO& Get();
  static glm::ivec2 GetViewportMousePosition();

public:
  bool ViewportIsHovered = false;
  glm::ivec2 ViewportSize;
  glm::ivec2 ViewportMin;
};
