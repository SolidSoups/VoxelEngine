#pragma once

#include <memory>
#include "Editor.h"

class PhysicsRuntime;
struct Texture;

class PhysicsEditor : public Editor {
public:
  PhysicsEditor(PhysicsRuntime* aRuntime);
  ~PhysicsEditor();
  void Draw() override;
  void DrawButtonsNotRunning();
  void DrawButtonsRunning(bool isPaused);

private:
  PhysicsRuntime* myRuntime;

  std::unique_ptr<Texture> myPlayTexture;
  std::unique_ptr<Texture> myPlayPausedTexture;
  std::unique_ptr<Texture> myPauseTexture;
  std::unique_ptr<Texture> myStopTexture;
  std::unique_ptr<Texture> myStepTexture;
};
