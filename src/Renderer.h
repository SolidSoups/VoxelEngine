#pragma once

#include <memory>
class NaiveCube;
class Shader;
struct Camera;

#define VERT_PATH "shaders/vert.glsl"
#define FRAG_PATH "shaders/frag.glsl"

class Renderer {
public:
  Renderer() = delete;
  static void Initialize();
  static void Destroy();
  static void DrawCube(const glm::vec3 &aPosition, const glm::mat4 &aTransform, const glm::vec3 &aColor);
  static void SetAspectRatio(float aAspectRatio);

private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
  static std::unique_ptr<Camera> myCamera;
  static float myAspectRatio;
};
