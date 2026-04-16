#pragma once

#include "Editor.h"

class ViewportEditor : public Editor {
public:
  void Draw(bool* aIsOpen=nullptr) override;

public:
  glm::ivec2 viewportSize;
};
