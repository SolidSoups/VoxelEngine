#pragma once

#include "rendering/TextureConfig.h"

struct Texture {
  uint32_t width = -1;
  uint32_t height = -1;
  uint32_t channels = -1;
  unsigned int id = 0;

public:
  Texture(const char* aFilePath, const TextureConfig& config = TextureConfig{});
  ~Texture();
};
