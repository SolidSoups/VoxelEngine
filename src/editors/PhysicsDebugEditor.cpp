#include "editors/PhysicsDebugEditor.h"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <algorithm>

#include "physics/PhysicsEngine.h"
#include "rendering/Renderer.h"
#include "voxel/VoxelType.h"

static constexpr int TEX_SIZE = CHUNK_SIZE;

PhysicsDebugEditor::PhysicsDebugEditor(PhysicsEngine &aEngine) : myEngine(aEngine)
{
    CreateTextures();
}

PhysicsDebugEditor::~PhysicsDebugEditor()
{
    glDeleteTextures(1, &myHeightMapTex);
    glDeleteTextures(1, &mySlopeMapTex);
    glDeleteTextures(1, &myMovedTex);
}

void PhysicsDebugEditor::CreateTextures()
{
    auto makeTexture = [](unsigned int &outTex, GLenum internalFormat, GLenum format, GLenum type)
    {
        glGenTextures(1, &outTex);
        glBindTexture(GL_TEXTURE_2D, outTex);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, TEX_SIZE, TEX_SIZE, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    };

    makeTexture(myHeightMapTex, GL_R8, GL_RED, GL_UNSIGNED_BYTE);
    makeTexture(mySlopeMapTex,  GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    makeTexture(myMovedTex,     GL_R8, GL_RED, GL_UNSIGNED_BYTE);
}

void PhysicsDebugEditor::UpdateTextures()
{
    PhysicsDebugData data = myEngine.GetDebugData();

    // make height map texture
    {
        uint8_t maxH = 1;
        for (int i = 0; i < TEX_SIZE * TEX_SIZE; i++)
            maxH = std::max(maxH, data.heightMap[i]);

        static uint8_t pixels[TEX_SIZE * TEX_SIZE];
        for (int z = 0; z < TEX_SIZE; z++)
            for (int x = 0; x < TEX_SIZE; x++)
                pixels[x + z * TEX_SIZE] = (uint8_t)((data.heightMap[x + z * TEX_SIZE] * 255) / maxH);

        glBindTexture(GL_TEXTURE_2D, myHeightMapTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RED, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // make slope direction texture
    {
        static uint8_t pixels[TEX_SIZE * TEX_SIZE * 4];
        for (int z = 0; z < TEX_SIZE; z++)
        {
            for (int x = 0; x < TEX_SIZE; x++)
            {
                glm::vec2 slope = data.slopeMap[x + z * TEX_SIZE];
                float     len   = glm::length(slope);
                int       idx   = (x + z * TEX_SIZE) * 4;

                // invalid slopes become RED
                if (len < 0.001f)
                {
                    pixels[idx + 0] = 255;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
                else
                {
                    // we need to normalize vectors (-1 -> 1) => (0 -> 1)
                    glm::vec2 n = slope / len;
                    pixels[idx + 0] = (uint8_t)((n.x * 0.5f + 0.5f) * 255);
                    pixels[idx + 1] = (uint8_t)((n.y * 0.5f + 0.5f) * 255);
                    pixels[idx + 2] = (uint8_t)(std::min(len * 50.f, 1.f) * 255);
                    pixels[idx + 3] = 255;
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, mySlopeMapTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // make 'movement' texture
    {
        static uint8_t pixels[TEX_SIZE * TEX_SIZE];
        for (int z = 0; z < TEX_SIZE; z++)
        {
            for (int x = 0; x < TEX_SIZE; x++)
            {
                VoxelBitset bits  = data.movedVoxels[x + z * TEX_SIZE];
                int         count = std::popcount(bits);
                pixels[x + z * TEX_SIZE] = (uint8_t)std::min(count * 64, 255);
            }
        }

        glBindTexture(GL_TEXTURE_2D, myMovedTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RED, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void PhysicsDebugEditor::Draw()
{
    ImGui::Begin("Physics Debug");
    ImGui::Checkbox("Enable", &myEnabled);

    if (!myEnabled)
    {
        Renderer::ClearDebugTexture();
        ImGui::End();
        return;
    }

    const char *modes[] = {"Height Map", "Slope Direction", "Moved This Frame"};
    int         current = (int)mySelectedMode - 1;
    ImGui::Combo("3D View", &current, modes, 3);
    mySelectedMode = (DebugMode)(current + 1);

    UpdateTextures();

    unsigned int activeTex = 0;
    switch (mySelectedMode)
    {
    case DebugMode::HeightMap:      activeTex = myHeightMapTex; break;
    case DebugMode::SlopeDirection: activeTex = mySlopeMapTex;  break;
    case DebugMode::MovedThisFrame: activeTex = myMovedTex;     break;
    default: break;
    }
    Renderer::SetDebugTexture(activeTex, mySelectedMode);

    constexpr float PADDING    = 8.f;
    constexpr float TEX_DISPLAY = 400.f;
    ImVec2          size{TEX_DISPLAY, TEX_DISPLAY};

    ImGui::BeginGroup();
    ImGui::Text("Height Map");
    ImGui::Image((ImTextureRef)(intptr_t)myHeightMapTex, size);
    ImGui::EndGroup();

    ImGui::SameLine(0.f, PADDING);
    ImGui::BeginGroup();
    ImGui::Text("Slope Direction");
    ImGui::Image((ImTextureRef)(intptr_t)mySlopeMapTex, size);
    ImGui::EndGroup();

    ImGui::SameLine(0.f, PADDING);
    ImGui::BeginGroup();
    ImGui::Text("Moved This Frame");
    ImGui::Image((ImTextureRef)(intptr_t)myMovedTex, size);
    ImGui::EndGroup();

    ImGui::End();
}
