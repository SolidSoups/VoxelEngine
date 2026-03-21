#pragma once

class Editor {
public:
  virtual ~Editor() = default;
  virtual void Draw() = 0;
};
