#pragma once
#include <glm/glm.hpp>

struct Framebuffer {
  unsigned int fbo = 0, texture = 0, depthStencil = 0;

public:
  Framebuffer() = default;
  Framebuffer(unsigned int aWidth, unsigned int aHeight);
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
};
