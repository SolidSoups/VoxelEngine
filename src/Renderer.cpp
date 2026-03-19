#include "Renderer.h"

#include "ApplicationLayer.h"

#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/Camera.h"
#include "objects/VoxelChunk.h"

std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;
std::unique_ptr<Camera> Renderer::myCamera = nullptr;
float Renderer::myAspectRatio = 1.0f;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();
  Renderer::myCamera = std::make_unique<Camera>();

  // set winding order
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::Destroy() {
  Renderer::myShader.reset();
  Renderer::myNaiveCube.reset();
  Renderer::myCamera.reset();
}

void Renderer::SetAspectRatio(float aAspectRatio) {
  Renderer::myAspectRatio = aAspectRatio;
}

void Renderer::DrawCube(const glm::vec3 &aPosition, const glm::mat4 &aTransform,
                        const glm::vec3 &aColor) {
  Renderer::myShader->Bind();

  // set uniforms
  Renderer::myShader->setMatrix4("view", Renderer::myCamera->getViewMatrix());
  Renderer::myShader->setMatrix4(
      "projection", Renderer::myCamera->getProjMatrix(Renderer::myAspectRatio));
  Renderer::myShader->setVec3("color", aColor);
  Renderer::myShader->setMatrix4("transform", aTransform);

  Renderer::myNaiveCube->Draw();
}
