#pragma once

#include "Editor.h"

class PhysicsRuntime;

class PhysicsEditor : public Editor {
public:
  PhysicsEditor(PhysicsRuntime* aRuntime) 
  : myRuntime(aRuntime){}
  void Draw() override;

private:
  PhysicsRuntime* myRuntime;
};
