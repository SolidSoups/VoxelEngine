#pragma once

#include <memory>
#include "Editor.h"

class PhysicsScheduler;
struct Texture;

class PhysicsEditor : public Editor {
public:
  PhysicsEditor(PhysicsScheduler* aRuntime);
  ~PhysicsEditor();
  void Draw() override;
  void DrawButtonsNotRunning();
  void DrawButtonsRunning(bool isPaused);

private:
  PhysicsScheduler* myRuntime;

  std::unique_ptr<Texture> myPlayTexture;
  std::unique_ptr<Texture> myPlayPausedTexture;
  std::unique_ptr<Texture> myPauseTexture;
  std::unique_ptr<Texture> myStopTexture;
  std::unique_ptr<Texture> myStepTexture;
};
