#pragma once

class Editor {
public:
  virtual ~Editor() = default;
  virtual void Draw(bool* aIsOpen=nullptr) = 0;
};
