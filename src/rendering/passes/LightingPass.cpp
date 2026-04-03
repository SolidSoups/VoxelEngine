#include "rendering/passes/LightingPass.h"

#include <glad/glad.h>

#include "rendering/Shader.h"

LightingPass::~LightingPass() {
  if (myQuadVAO)
    glDeleteVertexArrays(1, &myQuadVAO);
  if (myQuadVBO)
    glDeleteBuffers(1, &myQuadVBO);
}
void LightingPass::Initialize() {
  myShader = std::make_unique<Shader>("shaders/lighting.vert",
                                      "shaders/lighting.frag");
  framebuffer = Framebuffer(1000, 800);

  // create fullscreen quad
  float quadVerts[] = {-1, -1, 0, 0, 1, -1, 1, 0, 1,  1, 1, 1,
                       -1, -1, 0, 0, 1, 1,  1, 1, -1, 1, 0, 1};
  glGenVertexArrays(1, &myQuadVAO);
  glGenBuffers(1, &myQuadVBO);
  glBindVertexArray(myQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, myQuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}
void LightingPass::Execute(RenderpassInfo &someInfo) {
  // get geometry framebuffers
  Framebuffer *geometryFB = someInfo.framebuffers[0];

  myShader->Bind();

  // bind g-buffer textures from geometry pass
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, geometryFB->textures[0]); // position
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, geometryFB->textures[1]); // normal
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, geometryFB->textures[2]); // albedo

  myShader->setInt("gPosition", 0);
  myShader->setInt("gNormal", 1);
  myShader->setInt("gAlbedo", 2);

  glBindVertexArray(myQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
