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

void Renderer::DrawCube(glm::ivec3 aPosition, glm::vec3 color) {
  Renderer::myShader->Bind();

  // set the model matrix
  glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(aPosition.x, aPosition.y, aPosition.z));
  Renderer::myShader->setMatrix4("model", model);

  // set view and projection for shader
  glm::vec2 windowSize = ApplicationLayer::GetWindowSize();
  float aspect = windowSize.x / windowSize.y;
  Renderer::myShader->setMatrix4("view", Renderer::myCamera->getViewMatrix());
  Renderer::myShader->setMatrix4(
      "projection", Renderer::myCamera->getProjectionMatrix(aspect));

  // set color
  Renderer::myShader->setVec3("color", color);

  Renderer::myNaiveCube->Draw();
}
