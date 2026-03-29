#include "Renderer.h"

#include "ApplicationLayer.h"
#include "objects/Shader.h"
#include "objects/NaiveCube.h"
#include "objects/ViewportGrid.h"
#include "objects/Camera.h"
#include "objects/VoxelType.h"
#include "objects/Mesh.h"
#include "objects/VertexMode.h"
#include "objects/Framebuffer.h"
#include "objects/Renderpass.h"
#include "objects/GeometryPass.h"

#define DEFAULT_SCREEN_WIDTH 1000
#define DEFAULT_SCREEN_HEIGHT 800
std::unique_ptr<Shader> Renderer::myShader = nullptr;
std::unique_ptr<Shader> Renderer::myGridShader = nullptr;
std::unique_ptr<NaiveCube> Renderer::myNaiveCube = nullptr;
std::unique_ptr<ViewportGrid> Renderer::myViewportGrid = nullptr;
std::vector<std::unique_ptr<Renderpass>> Renderer::myRenderpasses;
Framebuffer Renderer::framebuffer;
bool Renderer::myIsWireframeMode = false;

void Renderer::Initialize() {
  Renderer::myShader = std::make_unique<Shader>(VERT_PATH, FRAG_PATH);
  Renderer::myGridShader =
      std::make_unique<Shader>(GRID_VERT_PATH, GRID_FRAG_PATH);
  Renderer::myNaiveCube = std::make_unique<NaiveCube>();
  Renderer::myViewportGrid = std::make_unique<ViewportGrid>();
  Renderer::framebuffer =
      Framebuffer(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

  // set the uVoxelColors uniforms of the voxel shader
  Renderer::myShader->Bind();
  glUniform3fv(
      glGetUniformLocation(Renderer::myShader->GetID(), "uVoxelColors"),
      ourVoxelColors.size(), &ourVoxelColors[0][0]);

  // initialize renderpasses
  myRenderpasses.push_back(std::make_unique<GeometryPass>());
  for (auto &pass : myRenderpasses)
    pass->Initialize();

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

void Renderer::SetWireframeMode(bool aMode) {
  if (aMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void Renderer::BeginFrame(Camera &camera, glm::ivec2 viewportSize) {
  Renderer::framebuffer.Resize(viewportSize.x, viewportSize.y);
  //
  // // clear the background
  // glClearColor(0, 0, 0, 1);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  // // update view and projection uniforms
  // Renderer::myShader->Bind();
  // Renderer::myShader->setMatrix4("view", camera.GetViewMatrix());
  // Renderer::myShader->setMatrix4("projection", camera.GetProjectionMatrix());
  //
  // Renderer::myGridShader->Bind();
  // Renderer::myGridShader->setMatrix4("view", camera.GetViewMatrix());
  // Renderer::myGridShader->setMatrix4("projection",
  //                                    camera.GetProjectionMatrix());
  // Renderer::myGridShader->setVec3("uCameraPos", camera.position);
}

void Renderer::RenderFrame(Camera &aCamera, Scene &aScene, glm::ivec2 viewportSize) {
  if(viewportSize.x <= 0 || viewportSize.y <= 0)
    return;

  WireframeVertexMode vertexMode;
  RenderpassInfo info{viewportSize, nullptr, aCamera, aScene, vertexMode};

  size_t size = myRenderpasses.size();
  for (size_t i = 0; i < size; i++) {
    bool isLast = (i == size - 1);
    Framebuffer &target =
        isLast ? Renderer::framebuffer : myRenderpasses[i]->framebuffer;

    target.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myRenderpasses[i]->Execute(info);
    target.Unbind();
    
    info.previous = &target;
  }
}

void Renderer::EndFrame() { Renderer::framebuffer.Unbind(); }

void Renderer::DrawVoxelGrid() {
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

void Renderer::DrawCube(const glm::mat4 &aTransform, int aVoxelType) {
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("uTransform", aTransform);
  Renderer::myShader->setInt("uVoxelType", aVoxelType);
  Renderer::myNaiveCube->Draw();
}

void Renderer::DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform,
                         int aVoxelType, IVertexMode &aVertexMode) {
  Renderer::myShader->Bind();
  Renderer::myShader->setMatrix4("uTransform", aTransform);
  Renderer::myShader->setInt("uVoxelType", aVoxelType);

  aVertexMode.OnEnable();
  aMesh.Draw();
  aVertexMode.OnDisable();
}
