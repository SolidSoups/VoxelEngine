#pragma once

#include <memory>
#include <glm/mat4x4.hpp>
#include "rendering/passes/Renderpass.h"
#include "rendering/Texture.h"


class IVertexMode;
class Shader;
class Scene;
struct Mesh;

struct GeometryPass : public Renderpass
{
  public:
    ~GeometryPass();
    void Initialize() override;
    void Execute(RenderpassInfo &someInfo) override;
    void DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform, IVertexMode &aVertexMode);

  private:
    std::unique_ptr<Shader> myShader;
    std::unique_ptr<Texture> myArrowTexture;
};
