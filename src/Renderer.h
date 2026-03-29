#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include "objects/Framebuffer.h"

class NaiveCube;
class ViewportGrid;
class Shader;
struct Mesh;
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
  static void BeginFrame(Camera &aCamera);
  static void EndFrame();
  static void Destroy();
  static void DrawCube(const glm::mat4 &aTransform, int aVoxelType);
  static void DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform, int aVoxelType, IVertexMode& aVertexMode);
  static void DrawVoxelGrid();
  static void SetWireframeMode(bool aMode);

  static Framebuffer frameBuffer;
private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<Shader> myGridShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
  static std::unique_ptr<ViewportGrid> myViewportGrid;
  static bool myIsWireframeMode;
};
