#include "Renderer.h"

#include <print>
#include "ApplicationLayer.h"

#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/Camera.h"

std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;
std::unique_ptr<Camera> Renderer::myCamera = nullptr;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();
  Renderer::myCamera = std::make_unique<Camera>();

  // set wining order
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::Destroy() {
  // nothing to do
}

void Renderer::DrawCube() {
  Renderer::myShader->Bind();

  // set view and projection for shader
  glm::vec2 windowSize = ApplicationLayer::GetWindowSize();
  float aspect = windowSize.x / windowSize.y;
  Renderer::myShader->setMatrix4("view", Renderer::myCamera->getViewMatrix());
  Renderer::myShader->setMatrix4(
      "projection", Renderer::myCamera->getProjectionMatrix(aspect));

  Renderer::myNaiveCube->Draw();
}
