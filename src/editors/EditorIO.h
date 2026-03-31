#pragma once
#include <glm/glm.hpp>

class EditorIO{
public:
  static EditorIO& Get();

public:
  bool ViewportIsHovered = false;
  glm::ivec2 ViewportSize;
  glm::ivec2 ViewportMin;
};
