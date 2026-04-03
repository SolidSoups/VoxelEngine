#pragma once

#include "rendering/Camera.h"
#include "rendering/Framebuffer.h"
#include "rendering/Scene.h"
#include "rendering/VertexMode.h"

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
