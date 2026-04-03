#pragma once

#include <memory>


#include "rendering/passes/Renderpass.h"
#include "voxel/VoxelBrushes.h"

class Shader;
class NaiveCube;

class PainterPass : public Renderpass {
  public:
  void Initialize() override;
  void Execute(RenderpassInfo& someInfo) override;
  inline bool ShouldClearColorBuffer() override { return false; }

private:
  std::unique_ptr<Shader> myShader;
  std::unique_ptr<NaiveCube> myNaiveCube;
  VoxelBrushes brushes;
};
