#pragma once

struct TextureConfig {
  bool mipmap = false;
  uint8_t mipLevels = 1;
  GLenum minFilter = GL_LINEAR;
  GLenum magFilter = GL_LINEAR;
  float lodBias = 0.0f;
  float lodMin = 0.0f;
  float lodMax = 1000.0f;
  int anisotropy = 1;
  GLenum wrapS = GL_REPEAT;
  GLenum wrapT = GL_REPEAT;
  bool flipY = false;
  TextureConfig() = default;
};
