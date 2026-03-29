#pragma once

#include <glm/mat4x4.hpp>
#include "Renderpass.h"
#include "Framebuffer.h"


class IVertexMode;
class Shader;
class Scene;
struct Mesh;

struct GeometryPass : public Renderpass {
public:
  void Initialize() override;
  void Execute(RenderpassInfo &someInfo) override;
  void DrawChunk(const Mesh& aMesh, const glm::mat4 &aTransform, IVertexMode &aVertexMode);

private:
  std::unique_ptr<Shader> myShader;
};
