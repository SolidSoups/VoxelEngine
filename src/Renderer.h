#pragma once

#include "objects/Shader.h"

#define VERT_PATH "shaders/vert.glsl"
#define FRAG_PATH "shaders/frag.glsl"

class Renderer {
public:
  Renderer() = delete;
  static void Initialize();
  static void Destroy();
  static void DrawCube();

private:
  static Shader myShader;
};

