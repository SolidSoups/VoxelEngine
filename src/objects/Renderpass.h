#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "../Scene.h"
#include "VertexMode.h"

struct RenderpassInfo {
  glm::ivec2 viewportSize;
  std::vector<Framebuffer*> framebuffers;
  unsigned int currentFramebufferIdx = 0;
  Camera& camera;
  Scene& scene;
  IVertexMode &vertexMode;
};

class Renderpass {
public:
  virtual ~Renderpass() = default;
  virtual void Initialize() = 0;
  virtual void Execute(RenderpassInfo& someInfo) = 0;
  virtual bool ShouldClearColorBuffer() { return true; }

  Framebuffer framebuffer;
};
