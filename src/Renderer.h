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
  static void DrawCube(glm::ivec3 aPosition, glm::vec3 color);

private:
  static std::unique_ptr<Shader> myShader;
  static std::unique_ptr<NaiveCube> myNaiveCube;
  static std::unique_ptr<Camera> myCamera;
};

