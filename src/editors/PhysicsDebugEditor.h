#pragma once

#include "Editor.h"

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
    bool           myEnabled = false;

    unsigned int myHeightMapTex = 0;
    unsigned int mySlopeMapTex  = 0;
    unsigned int myMovedTex     = 0;
};
