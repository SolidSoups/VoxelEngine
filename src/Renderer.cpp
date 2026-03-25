#include "Renderer.h"

#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/ViewportGrid.h"
#include "objects/Camera.h"
#include "objects/VoxelType.h"
#include "objects/Mesh.h"
#include "objects/VertexMode.h"

std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<Shader> Renderer::myGridShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;
std::unique_ptr<ViewportGrid> Renderer::myViewportGrid = nullptr;
bool Renderer::myIsWireframeMode = false;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myGridShader = std::make_unique<Shader>(GRID_VERT_PATH, GRID_FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();
  Renderer::myViewportGrid = std::make_unique<ViewportGrid>();

  // set the uVoxelColors uniforms of the voxel shader
  Renderer::myShader->Bind();
  glUniform3fv(glGetUniformLocation(Renderer::myShader->GetID(), "uVoxelColors"), ourVoxelColors.size(), &ourVoxelColors[0][0]);

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


void Renderer::SetWireframeMode(bool aMode){
  if(aMode){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
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
  glDisable(GL_CULL_FACE);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0f);
  Renderer::myGridShader->Bind();
  Renderer::myViewportGrid->Draw();   
  glEnable(GL_CULL_FACE);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);
}

void Renderer::DrawCube(const glm::mat4 &aTransform,
                        int aVoxelType) {
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("uTransform", aTransform);
  Renderer::myShader->setInt("uVoxelType", aVoxelType);
  Renderer::myNaiveCube->Draw();
}

void Renderer::DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform, int aVoxelType, IVertexMode& aVertexMode){
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("uTransform", aTransform);
  Renderer::myShader->setInt("uVoxelType", aVoxelType);  

  aVertexMode.OnEnable();
  aMesh.Draw();
  aVertexMode.OnDisable();
}
