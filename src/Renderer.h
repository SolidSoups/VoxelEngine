#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include "objects/Framebuffer.h"

class NaiveCube;
class ViewportGrid;
class Renderpass;
class Shader;
struct Mesh;
class Scene;
struct Camera;
class IVertexMode;

#define VERT_PATH "shaders/vert.glsl"
#define FRAG_PATH "shaders/frag.glsl"
#define GRID_VERT_PATH "shaders/grid_vert.glsl"
#define GRID_FRAG_PATH "shaders/grid_frag.glsl"

class Renderer {
public:
  Renderer() = delete;
  static void Initialize();
  static void InitializeRenderpasses(Scene& aScene);
  static void BeginFrame(Camera &aCamera, glm::ivec2 viewportSize);
  static void EndFrame();
  static void Destroy();
  static void RenderFrame(Camera& aCamera, Scene& aScene, glm::ivec2 viewportSize);
  static void DrawCube(const glm::mat4 &aTransform, int aVoxelType);
  static void DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform, int aVoxelType, IVertexMode& aVertexMode);
  static void DrawVoxelGrid();
  static void SetWireframeMode(bool aMode);

  static Framebuffer framebuffer;
private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<Shader> myGridShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
  static std::unique_ptr<ViewportGrid> myViewportGrid;
  static std::vector<std::unique_ptr<Renderpass>> myRenderpasses;
  static bool myIsWireframeMode;
};
