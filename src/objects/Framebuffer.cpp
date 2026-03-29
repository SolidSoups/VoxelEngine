#include "Framebuffer.h"

#include <print>

Framebuffer::Framebuffer(unsigned int aWidth, unsigned int aHeight)
    : myWidth(aWidth), myHeight(aHeight) {
  Create();
}

Framebuffer::~Framebuffer() { Destroy(); }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept {
  fbo = other.fbo;
  texture = other.texture;
  depthStencil = other.depthStencil;
  myWidth = other.myWidth;
  myHeight = other.myHeight;
  other.fbo = 0;
  other.texture = 0;
  other.depthStencil = 0;
}
Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
  if (this != &other) {
    Destroy();
    fbo = other.fbo;
    texture = other.texture;
    depthStencil = other.depthStencil;
    myWidth = other.myWidth;
    myHeight = other.myHeight;
    other.fbo = 0;
    other.texture = 0;
    other.depthStencil = 0;
  }
  return *this;
}

void Framebuffer::Create() {
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Always generate one texture attachment
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, myWidth, myHeight, 0, GL_RGB,
               GL_UNSIGNED_INT, nullptr);

  // set filtering to interpolation
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // attach texture to color attachment 0, which is texture output
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);

  // create depth and stencil buffer
  glGenRenderbuffers(1, &depthStencil);
  glBindRenderbuffer(GL_RENDERBUFFER, depthStencil);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, myWidth,
                        myHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, depthStencil);

  // check completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::println(stderr, "Framebuffer is incomplete!");
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, myWidth, myHeight);
}
void Framebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void Framebuffer::Resize(unsigned int newWidth, unsigned int newHeight) {
  if (myWidth == newWidth and myHeight == newHeight)
    return;

  myWidth = newWidth;
  myHeight = newHeight;
  Destroy();
  Create();
}
void Framebuffer::Destroy() {
  if (fbo)
    glDeleteFramebuffers(1, &fbo);
  if (texture)
    glDeleteTextures(1, &texture);
  if (depthStencil)
    glDeleteRenderbuffers(1, &depthStencil);
}
