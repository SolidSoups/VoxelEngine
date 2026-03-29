#pragma once

#include "Framebuffer.h"

struct Camera;
class IVertexMode;
class Scene;

struct RenderpassInfo {
  glm::ivec2 viewportSize;
  Framebuffer *previous;
  Camera& camera;
  Scene& scene;
  IVertexMode &vertexMode;
};

class Renderpass {
public:
  virtual ~Renderpass() = default;
  virtual void Initialize() = 0;
  virtual void Execute(RenderpassInfo& someInfo) = 0;

  Framebuffer framebuffer;
};
