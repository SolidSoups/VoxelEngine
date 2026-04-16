#include "TextureViewer.h"
#include "glm/common.hpp"

#include <glad/glad.h>

namespace
{
void makeTexture(unsigned int &outTex, GLenum internalFormat, GLenum format, GLenum type, GLenum width, GLenum height)
{
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
};
} // namespace

TextureEntry_uint8::TextureEntry_uint8(uint8_t           *someData,
                                       const std::string &aName,
                                       unsigned int       aWidth,
                                       unsigned int       aHeight)
    : TextureEntry(aName, aWidth, aHeight), data(someData)
{
    makeTexture(id, GL_R8, GL_RED, GL_UNSIGNED_BYTE, width, height);
}
TextureEntry_uint8::~TextureEntry_uint8()
{
    glDeleteTextures(1, &id);
}
unsigned int TextureEntry_uint8::Build()
{
    if (id == 0 or data == nullptr or myPixels == nullptr)
        return 0;

    const size_t size = width * height;

    // find max height
    uint8_t maxHeight = 1;
    for (int i = 0; i < size; i++)
    {
        maxHeight = glm::max(maxHeight, data[i]);
    }

    // create red texture
    for (int i = 0; i < size; i++)
        myPixels[i] = (uint8_t) ((data[i] * 255) / maxHeight);

    // update gpu representation
    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, myPixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

// // handles vec2 values
// struct TextureEntry_vec2 : public TextureEntry {
//     glm::vec2* data = nullptr;
//
//     TextureEntry_vec2(glm::vec2* someData, const std::string& aName, unsigned int aWidth, unsigned int aHeight);
//     ~TextureEntry_vec2();
//
//     unsigned int Build() override;
// };

TextureEntry_vec2::TextureEntry_vec2(glm::vec2         *someData,
                                     const std::string &aName,
                                     unsigned int       aWidth,
                                     unsigned int       aHeight)
    : TextureEntry(aName, aWidth, aHeight, 4), data(someData)
{
    makeTexture(id, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, aWidth, aHeight);
}
TextureEntry_vec2::~TextureEntry_vec2()
{
    glDeleteTextures(1, &id);
}

unsigned int TextureEntry_vec2::Build()
{
    if (id == 0 or data == nullptr or myPixels == nullptr)
        return 0;

    const size_t size = width * height;

    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            glm::vec2 slope = data[x + z * height];
            float     len   = glm::length(slope);
            int       idx   = (x + z * height) * 4;

            // invalid slopes become RED
            if (len < 0.001f)
            {
                myPixels[idx + 0] = 255;
                myPixels[idx + 1] = 0;
                myPixels[idx + 2] = 0;
                myPixels[idx + 3] = 255;
            }
            else
            {
                glm::vec2 n     = slope / len;
                myPixels[idx + 0] = (uint8_t) ((n.x * 0.5f + 0.5f) * 255);
                myPixels[idx + 1] = (uint8_t) ((n.y * 0.5f + 0.5f) * 255);
                myPixels[idx + 2] = (uint8_t) (std::min(len * 50.f, 1.f) * 255);
                myPixels[idx + 3] = 255;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, myPixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}
