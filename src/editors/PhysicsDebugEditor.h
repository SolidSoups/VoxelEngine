#pragma once

#include "Editor.h"
#include "rendering/Renderer.h"

class PhysicsEngine;

class PhysicsDebugEditor : public Editor
{
  public:
    PhysicsDebugEditor(PhysicsEngine &aEngine);
    ~PhysicsDebugEditor();

    void Draw() override;

  private:
    void CreateTextures();
    void UpdateTextures();

    PhysicsEngine &myEngine;
    bool           myEnabled      = false;
    DebugMode      mySelectedMode = DebugMode::SlopeDirection;

    unsigned int myHeightMapTex = 0;
    unsigned int mySlopeMapTex  = 0;
    unsigned int myFDASlopeMapTex = 0;
    unsigned int myMovedTex     = 0;
    unsigned int myActionMapTex    = 0;
};
