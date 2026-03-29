#include "GeometryPass.h"

#include "Mesh.h"
#include "VertexMode.h"
#include "Shader.h"
#include "Camera.h"
#include "../Scene.h"
#include "VoxelType.h"

void GeometryPass::Initialize() {
  myShader = std::make_unique<Shader>("shaders/geometry.vert",
                                      "shaders/geometry.frag");
  myShader->Bind();
  glUniform3fv(glGetUniformLocation(myShader->GetID(), "uVoxelColors"),
               ourVoxelColors.size(), &ourVoxelColors[0][0]);
  myShader->Unbind();

  // G-Buffer attachments
  framebuffer =
      Framebuffer(1000, 800,
                  {
                      {GL_RGB16F, GL_RGB, GL_FLOAT},      // position
                      {GL_RGB16F, GL_RGB, GL_FLOAT},      // normal
                      {GL_RGBA, GL_RGBA, GL_UNSIGNED_INT} // albedo/color
                  });
}
void GeometryPass::Execute(RenderpassInfo &someInfo) {
  myShader->Bind();
  myShader->setMatrix4("uView", someInfo.camera.GetViewMatrix());
  myShader->setMatrix4("uProjection", someInfo.camera.GetProjectionMatrix());

  // draw the chunk
  auto &chunk = someInfo.scene.GetVoxelChunk();
  myShader->setMatrix4("uTransform", chunk.GetTransform());
  someInfo.vertexMode.OnEnable();
  someInfo.scene.GetVoxelMesh().Draw();
  someInfo.vertexMode.OnDisable();
}
void DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform,
               IVertexMode &aVertexMode) {}
