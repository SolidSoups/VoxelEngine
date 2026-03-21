#include "Texture.h"

#include <print>

#include <glad/glad.h>

#include "../helpers/StbImage.h"

Texture::Texture(const char *aFilePath, const TextureConfig &aConfig) {
  StbImage image = StbImage(aFilePath, 0, aConfig.flipY);
  if (!image.data) {
    std::print(stderr, "No image data in file {0}", aFilePath);
    return;
  }

  // determine channels
  GLenum internalFormat = 0, dataFormat = 0;
  switch (image.channels) {
  case 1:
    internalFormat = GL_RED;
    dataFormat = GL_RED;
    break;
  case 3:
    internalFormat = GL_RGB8;
    dataFormat = GL_RGB;
    break;
  case 4:
    internalFormat = GL_RGBA8;
    dataFormat = GL_RGBA;
    break;
  default:
    std::print(stderr, "Unknown image channel count: {0}", image.channels);
    return;
  }

  // create texture object
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  // set texture params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, aConfig.wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, aConfig.wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aConfig.minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aConfig.magFilter);

  // LOD params
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, aConfig.lodBias);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, aConfig.lodMin);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, aConfig.lodMax);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, aConfig.mipLevels);

  // anisotropy filtering (if supported)
  if (aConfig.anisotropy > 1) {
    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
    GLfloat aniso = std::min((float)aConfig.anisotropy, maxAnisotropy);
    if (aniso != aConfig.anisotropy) {
      std::print("Overode anisotropy of {0} with max of {1}",
                 aConfig.anisotropy, aniso);
    }
    glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY, aniso);
  }

  // disable row padding (stbi provides tightly packed pixel data)
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // upload pixel data
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat,
               GL_UNSIGNED_INT, image.data);

  // generate mip maps
  if(aConfig.mipmap){
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  // unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  this->width = image.width;
  this->height = image.height;
  this->channels = image.channels;
}
