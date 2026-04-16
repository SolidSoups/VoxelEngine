#include "TextureViewer.h"

#include <imgui.h>

void TextureViewer::Draw(bool* aIsOpen)
{
    ImGui::Begin("Texture Viewer", aIsOpen);
    DrawList();
    ImGui::SameLine();
    DrawTextures();
    ImGui::End();
}

void TextureViewer::DrawTextures()
{
    // get the selected texture
    TextureEntry *textureEntry = nullptr;
    if (selectedTexture <= -1)
    {
        return;
    }
    textureEntry = data.textures[selectedTexture].get();
    unsigned int texId = textureEntry->Build();

    static float  zoom = 12.0f;
    static ImVec2 pan  = ImVec2(0, 0);
    static ImVec2 panOrigin;
    static bool   panning = false;

    ImGui::BeginChild(
        "##imgpanel", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    ImVec2 canvasPos  = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    ImGui::InvisibleButton("##canvas", canvasSize);
    ImGuiIO &io = ImGui::GetIO();

    // pan on right drag
    if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        pan.x += io.MouseDelta.x;
        pan.y += io.MouseDelta.y;
    }

    // zoom on scroll
    if (ImGui::IsItemHovered())
    {
        if (io.MouseWheel != 0)
        {
            float  zoomDelta = io.MouseWheel * 0.1f * zoom;
            float oldZoom = zoom;
            zoom = std::max(0.1f, zoom + zoomDelta);
            float ratio = zoom / oldZoom;

            ImVec2 mouseRel  = ImVec2(io.MousePos.x - canvasPos.x, io.MousePos.y - canvasPos.y);
            pan.x = mouseRel.x * (1.0f - ratio) + pan.x * ratio;
            pan.y = mouseRel.y * (1.0f - ratio) + pan.y * ratio;
        }
    }


    // Draw
    ImVec2 imgSize = ImVec2(textureEntry->width * zoom, textureEntry->height * zoom);
    ImVec2 imgPos  = ImVec2(canvasPos.x + pan.x, canvasPos.y + pan.y);

    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->PushClipRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), true);
    dl->AddImage((ImTextureRef) (intptr_t) texId, imgPos, ImVec2(imgPos.x + imgSize.x, imgPos.y + imgSize.y));
    dl->PopClipRect();

    ImGui::EndChild();
}

void TextureViewer::DrawList()
{
    static int selected      = 0;
    size_t     texturesCount = data.textures.size();
    if (texturesCount == 0)
    {
        selectedTexture = -1;
    }
    else if (selectedTexture == -1)
        selectedTexture = 0;

    const char *items[texturesCount];
    for (int i = 0; i < texturesCount; i++)
    {
        if (!data.textures[i])
            items[i] = "nullptr";
        items[i] = data.textures[i]->displayName.c_str();
    }

    // draw header
    ImGui::Text("Textures");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    constexpr int MAX_SIZE = 30;
    ImVec2        listSize = ImVec2(200, ImGui::GetContentRegionAvail().y);
    if (ImGui::BeginListBox("##mylist", listSize))
    {
        for (int i = 0; i < texturesCount; i++)
        {
            ImGui::PushID(i);
            bool isSelected = (selected == i);
            if (ImGui::Selectable(items[i], isSelected))
            {
                selected        = i;
                selectedTexture = i;
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);
}
