#pragma once

#include <print>
#include <stb_image.h>

struct StbImage {
  unsigned char *data = nullptr;
  int width = -1, height = -1, channels = -1;

  StbImage() = default;
  explicit StbImage(const char *aFilePath, int aForceChannels = 0, bool aFlip = true) {
    stbi_set_flip_vertically_on_load(aFlip);

    // load image using stbi
    data = stbi_load(aFilePath, &width, &height, &channels, aForceChannels);

    // double check load
    if (!data) {
      std::print(stderr, "stbi_load returned null:\n{0}", stbi_failure_reason());
      return;
    }
  }
  ~StbImage() {
    if (data)
      stbi_image_free(data);
  }

  // no copy
  StbImage(const StbImage &) = delete;
  StbImage &operator=(const StbImage &) = delete;
  // move ok
  StbImage(StbImage &&other) noexcept
      : data(other.data), width(other.width), height(other.height),
        channels(other.channels) {
    other.data = nullptr;
  }
  StbImage &operator=(StbImage &&other) noexcept {
    if (this != &other) {
      if (data)
        stbi_image_free(data);
      data = other.data;
      width = other.width;
      height = other.height;
      channels = other.channels;
      other.data = nullptr;
    }
    return *this;
  }
};
