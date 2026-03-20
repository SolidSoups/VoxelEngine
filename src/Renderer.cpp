#include "Renderer.h"

#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/ViewportGrid.h"
#include "objects/Camera.h"

std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<Shader> Renderer::myGridShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;
std::unique_ptr<ViewportGrid> Renderer::myViewportGrid = nullptr;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myGridShader = std::make_unique<Shader>(GRID_VERT_PATH, GRID_FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();
  Renderer::myViewportGrid = std::make_unique<ViewportGrid>();

  // set winding order
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::Destroy() {
  Renderer::myShader.reset();
  Renderer::myGridShader.reset();
  Renderer::myNaiveCube.reset();
  Renderer::myViewportGrid.reset();
}

void Renderer::BeginFrame(Camera &camera) {
  // clear the background
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update view and projection uniforms
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("view", camera.GetViewMatrix());
  Renderer::myShader->setMatrix4("projection", camera.GetProjectionMatrix());

  Renderer::myGridShader->Bind();
  Renderer::myGridShader->setMatrix4("view", camera.GetViewMatrix());
  Renderer::myGridShader->setMatrix4("projection", camera.GetProjectionMatrix());
  Renderer::myGridShader->setVec3("uCameraPos", camera.position);
}

void Renderer::DrawVoxelGrid(){
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Renderer::myGridShader->Bind();
  Renderer::myViewportGrid->Draw();   
  glDisable(GL_BLEND);
}

void Renderer::DrawCube(const glm::mat4 &aTransform,
                        const glm::vec3 &aColor) {
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("transform", aTransform);
  Renderer::myShader->setVec3("color", aColor);
  Renderer::myNaiveCube->Draw();
}
