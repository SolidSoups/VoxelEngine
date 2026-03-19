#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

class NaiveCube;
class Shader;
struct Camera;

#define VERT_PATH "shaders/vert.glsl"
#define FRAG_PATH "shaders/frag.glsl"

class Renderer {
public:
  Renderer() = delete;
  static void Initialize();
  static void BeginFrame(Camera& aCamera);
  static void Destroy();
  static void DrawCube(
    const glm::mat4 &aTransform, const glm::vec3 &aColor
  );

private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
};
