#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

class NaiveCube;
class ViewportGrid;
class Shader;
struct Camera;

#define VERT_PATH "shaders/vert.glsl"
#define FRAG_PATH "shaders/frag.glsl"
#define GRID_VERT_PATH "shaders/grid_vert.glsl"
#define GRID_FRAG_PATH "shaders/grid_frag.glsl"

class Renderer {
public:
  Renderer() = delete;
  static void Initialize();
  static void BeginFrame(Camera& aCamera);
  static void Destroy();
  static void DrawCube(
    const glm::mat4 &aTransform, const glm::vec3 &aColor
  );
  static void DrawVoxelGrid();

private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<Shader> myGridShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
  static std::unique_ptr<ViewportGrid> myViewportGrid;
};
