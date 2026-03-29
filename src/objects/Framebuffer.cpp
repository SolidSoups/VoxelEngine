#include "Framebuffer.h"

#include <print>

Framebuffer::Framebuffer(unsigned int aWidth, unsigned int aHeight,
                         std::vector<AttachmentSpec> someAttachments)
    : myWidth(aWidth), myHeight(aHeight), myAttachments(someAttachments) {
  Create();
}

Framebuffer::~Framebuffer() { Destroy(); }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept {
  fbo = other.fbo;
  textures = std::move(other.textures);
  myAttachments = std::move(other.myAttachments);
  depthStencil = other.depthStencil;
  myWidth = other.myWidth;
  myHeight = other.myHeight;
  other.fbo = 0;
  other.textures.clear();
  other.myAttachments.clear();
  other.depthStencil = 0;
}
Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
  if (this != &other) {
    Destroy();
  fbo = other.fbo;
  textures = std::move(other.textures);
  myAttachments = std::move(other.myAttachments);
  depthStencil = other.depthStencil;
  myWidth = other.myWidth;
  myHeight = other.myHeight;
  other.fbo = 0;
  other.textures.clear();
  other.myAttachments.clear();
  other.depthStencil = 0;
  }
  return *this;
}

void Framebuffer::Create() {
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  
  textures.resize(myAttachments.size());
  glGenTextures(myAttachments.size(), textures.data());

  std::vector<unsigned int> drawBuffers;
  for(size_t i=0; i<myAttachments.size(); i++){
    auto& spec = myAttachments[i];
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, spec.internalFormat, myWidth, myHeight,
                 0, spec.format, spec.type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                           GL_TEXTURE_2D, textures[i], 0);
    drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
  }
  
  // tell opengl which buffers to draw to
  glDrawBuffers(drawBuffers.size(), drawBuffers.data());

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
  if(!textures.empty())
    glDeleteTextures(textures.size(), textures.data());
  if (depthStencil)
    glDeleteRenderbuffers(1, &depthStencil);
  textures.clear();
}
