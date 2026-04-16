#pragma once

#include "editors/Editor.h"
class KeybindsEditor : public Editor {
public:
  void Draw(bool* aIsOpen=nullptr) override;
};
