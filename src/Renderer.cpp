#include "Renderer.h"

#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/Camera.h"

std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();

  // set winding order
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::Destroy() {
  Renderer::myShader.reset();
  Renderer::myNaiveCube.reset();
}

void Renderer::BeginFrame(Camera &camera) {
  // clear the background
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update view and projection uniforms
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("view", camera.GetViewMatrix());
  Renderer::myShader->setMatrix4("projection", camera.GetProjectionMatrix());
}

void Renderer::DrawCube(const glm::mat4 &aTransform,
                        const glm::vec3 &aColor) {
  Renderer::myShader->setMatrix4("transform", aTransform);
  Renderer::myShader->setVec3("color", aColor);
  Renderer::myNaiveCube->Draw();
}
