#pragma once

#include "rendering/passes/Renderpass.h"

class Shader;

class LightingPass : public Renderpass {
public:
  ~LightingPass();
  void Initialize() override;
  void Execute(RenderpassInfo &someInfo) override;

private:
  std::unique_ptr<Shader> myShader;
  unsigned int myQuadVAO = 0, myQuadVBO = 0;
};
