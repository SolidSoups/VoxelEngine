#include "rendering/passes/PainterPass.h"

#include "rendering/passes/Renderpass.h"
#include "rendering/Shader.h"
#include "rendering/Camera.h"
#include "rendering/NaiveCube.h"
#include <memory>
#include "voxel/VoxelPainter.h"

void PainterPass::Initialize(){
  myShader = std::make_unique<Shader>(
    "shaders/painter.vert",
    "shaders/painter.frag"
  );
  myNaiveCube = std::make_unique<NaiveCube>();
  brushes.PrecomputeSphere(1);
  brushes.PrecomputeSphere(2);
  brushes.PrecomputeSphere(3);
  brushes.PrecomputeSphere(4);
}
void PainterPass::Execute(RenderpassInfo& someInfo){

  // get current and geometry framebuffers
  Framebuffer* geometryFB = someInfo.framebuffers[0];
  Framebuffer* lightingFB = someInfo.framebuffers[1];
  Framebuffer* currentFB = someInfo.framebuffers[someInfo.currentFramebufferIdx];
  int w = someInfo.viewportSize.x;
  int h = someInfo.viewportSize.y;

  // blit lighting color into final framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, lightingFB->fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentFB->fbo);
  glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  // blit geometry depth into final framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryFB->fbo);
  glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  auto& paintState = PainterState::Get();
  if(!paintState.drawCursor)
    return;

  auto sphereMesh = brushes.GetSphere(paintState.radius);

  // rebind current framebuffer
  currentFB->Bind();

  glEnable(GL_DEPTH_TEST);

  // enable blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // push cursor slightly forward in depth, to avoid z-fighting with voxel geometry
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1.0f, -1.0f);

  // bind shader and set uniforms
  myShader->Bind();
  myShader->setMatrix4("uView", someInfo.camera.GetViewMatrix());
  myShader->setMatrix4("uProjection", someInfo.camera.GetProjectionMatrix());

  // create transform matrix
  glm::vec3& cursorPos = paintState.cursorPosition;
  glm::mat4 transform = glm::translate(glm::mat4(1), cursorPos);
  transform = glm::scale(transform, glm::vec3(4.0f / CHUNK_SIZE));
  myShader->setMatrix4("uTransform", transform);

  // myNaiveCube->Draw();
  sphereMesh->Draw();
  

  glDisable(GL_BLEND);
  glDisable(GL_POLYGON_OFFSET_FILL);

  myShader->Unbind();
}
