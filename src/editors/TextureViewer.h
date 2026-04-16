#pragma once

#include <string>
#include "editors/Editor.h"

#define NULL_TEXTURE 0

// texture types

// base class
struct TextureEntry
{
    std::string  displayName = "";
    unsigned int id = 0, width = 0, height = 0, channels = 0;

    TextureEntry(const std::string &aName, unsigned int aWidth, unsigned int aHeight, unsigned int aChannels = 1)
        : displayName(aName), width(aWidth), height(aHeight), channels(aChannels)
    {
        myPixels = new uint8_t[width * height * channels]();
    }
    virtual ~TextureEntry()
    {
        delete[] myPixels;
    }

    // build the texture and return a texture pointer
    virtual unsigned int Build()
    {
        return 0;
    }

  protected:
    uint8_t *myPixels;
};
// handles flat uint8 textures
struct TextureEntry_uint8 : public TextureEntry
{
    uint8_t *data = nullptr;

    TextureEntry_uint8(uint8_t *someData, const std::string &aName, unsigned int aWidth, unsigned int aHeight);
    ~TextureEntry_uint8();

    unsigned int Build() override;
};
// handles vec2 values
struct TextureEntry_vec2 : public TextureEntry
{
    glm::vec2 *data = nullptr;

    TextureEntry_vec2(glm::vec2 *someData, const std::string &aName, unsigned int aWidth, unsigned int aHeight);
    ~TextureEntry_vec2();

    unsigned int Build() override;
};

// Data class accessible from anywhere
struct TextureViewerData
{
    static TextureViewerData &Get()
    {
        static TextureViewerData instance;
        return instance;
    }

    template <typename T, typename... Args> void AddEntry(Args &&...args)
    {
        textures.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    std::vector<std::unique_ptr<TextureEntry>> textures{};
};

// Editor class
class TextureViewer : public Editor
{
  public:
    void Draw() override;

  private:
    void DrawList();
    void DrawTextures();

    int                selectedTexture = -1;
    TextureViewerData &data            = TextureViewerData::Get();
};
