#pragma once
#include <glm/glm.hpp>

struct AttachmentSpec {
  unsigned int internalFormat;
  unsigned int format;
  unsigned int type;
};

struct Framebuffer {
  unsigned int fbo = 0, depthStencil = 0;
  std::vector<unsigned int> textures;

public:
  Framebuffer() = default;
  Framebuffer(unsigned int aWidth, unsigned int aHeight, std::vector<AttachmentSpec> someAttachments={{GL_RGB, GL_RGB, GL_UNSIGNED_INT}});
  ~Framebuffer();
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&&) noexcept;
  Framebuffer& operator=(Framebuffer&&) noexcept;

  void Create();
  void Bind();
  void Unbind();
  void Resize(unsigned int newWidth, unsigned int newHeight);
  void Destroy();
  inline unsigned int GetWidth() const {
    return myWidth;
  }
  inline unsigned int GetHeight() const {
    return myHeight;
  }
  inline float GetAspectRatio() const {
    return (float)myWidth / myHeight;
  }

private:
  unsigned int myWidth = 0, myHeight = 0;
  std::vector<AttachmentSpec> myAttachments;
};
