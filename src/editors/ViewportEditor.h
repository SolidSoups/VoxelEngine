#pragma once

#include "Editor.h"

class ViewportEditor : public Editor {
public:
  void Draw() override;

public:
  glm::ivec2 viewportSize;
};
