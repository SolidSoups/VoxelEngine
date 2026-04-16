#include "TextureViewer.h"

#include <imgui.h>

void TextureViewer::Draw(bool* aIsOpen)
{
    ImGui::Begin("Texture Viewer", aIsOpen);
    DrawList(ImGui::IsWindowFocused(ImGuiHoveredFlags_ChildWindows));
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

    std::string textureHeader = "Texture: ";
    if(textureEntry){
        textureHeader += textureEntry->displayName;
    }
    else{
        textureHeader += "None";
    }
    ImGui::BeginChild(
        "##imgpanel", ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("%s", textureHeader.c_str());
    ImGui::SetWindowFontScale(1.0f);

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

    // Vec2 direction color wheel guide
    if (TextureEntry_vec2 *vec2Entry = dynamic_cast<TextureEntry_vec2 *>(textureEntry))
    {
        constexpr float kTwoPI = 6.28318530717958647692f;
        const float     radius = 40.0f;
        ImVec2          center = {canvasPos.x + canvasSize.x - radius - 24.0f,
                                  canvasPos.y + canvasSize.y - radius - 36.0f};

        dl->AddCircleFilled(center, radius + 4.0f, IM_COL32(0, 0, 0, 160));

        constexpr int kSegments = 64;
        for (int i = 0; i < kSegments; i++)
        {
            float a0 = (i / (float) kSegments) * kTwoPI;
            float a1 = ((i + 1) / (float) kSegments) * kTwoPI;
            float nx = cosf(a0), ny = sinf(a0);
            ImU32 col = IM_COL32((int) ((nx * 0.5f + 0.5f) * 255), (int) ((ny * 0.5f + 0.5f) * 255), 200, 255);
            dl->AddTriangleFilled(center, {center.x + cosf(a0) * radius, center.y + sinf(a0) * radius},
                                  {center.x + cosf(a1) * radius, center.y + sinf(a1) * radius}, col);
        }

        dl->AddCircle(center, radius, IM_COL32(220, 220, 220, 200));

        dl->AddText({center.x + radius + 4.0f, center.y - 7.0f}, IM_COL32_WHITE, "+X");
        dl->AddText({center.x - radius - 20.0f, center.y - 7.0f}, IM_COL32_WHITE, "-X");
        dl->AddText({center.x - 7.0f, center.y + radius + 4.0f}, IM_COL32_WHITE, "+Y");
        dl->AddText({center.x - 7.0f, center.y - radius - 16.0f}, IM_COL32_WHITE, "-Y");

        float swatchTop = center.y + radius + 22.0f;
        dl->AddRectFilled({center.x - 20.0f, swatchTop}, {center.x - 8.0f, swatchTop + 12.0f},
                          IM_COL32(255, 0, 0, 255));
        dl->AddText({center.x - 4.0f, swatchTop}, IM_COL32_WHITE, "= no flow");

        // Sample vec2 under cursor and mark its direction on the wheel
        if (ImGui::IsItemHovered() && vec2Entry->data != nullptr)
        {
            int tx = (int) ((io.MousePos.x - canvasPos.x - pan.x) / zoom);
            int ty = (int) ((io.MousePos.y - canvasPos.y - pan.y) / zoom);

            if (tx >= 0 && tx < (int) textureEntry->width && ty >= 0 && ty < (int) textureEntry->height)
            {
                glm::vec2 sample = vec2Entry->data[tx + ty * textureEntry->height];
                float     len    = glm::length(sample);

                if (len > 0.001f)
                {
                    glm::vec2 n      = sample / len;
                    float     angle  = atan2f(n.y, n.x);
                    ImVec2    marker = {center.x + cosf(angle) * radius * 0.8f,
                                        center.y + sinf(angle) * radius * 0.8f};
                    dl->AddLine(center, marker, IM_COL32_WHITE, 1.5f);
                    dl->AddCircleFilled(marker, 5.0f, IM_COL32_WHITE);
                    dl->AddCircle(marker, 5.0f, IM_COL32(0, 0, 0, 200), 12);
                }
                else
                {
                    dl->AddCircleFilled(center, 5.0f, IM_COL32(255, 50, 50, 255));
                }
            }
        }
    }

    dl->PopClipRect();

    ImGui::EndChild();
    ImGui::EndChild();
}

void TextureViewer::DrawList(bool aIsWindowHovered)
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

    if(aIsWindowHovered && texturesCount > 0){
        if(ImGui::IsKeyPressed(ImGuiKey_UpArrow) and selected > 0){
            selected--;
            selectedTexture = selected;
        }
        if(ImGui::IsKeyPressed(ImGuiKey_DownArrow) and selected < (int)texturesCount - 1){
            selected++;
            selectedTexture = selected;
        }
    }
}
