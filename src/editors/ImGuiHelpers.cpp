#include "ImGuiHelpers.h"

#include <imgui.h>

bool ImGuiHelpers::DrawFilterCombo(const char *aName, const char *someItems[],
                                   int aCount, int &aSelectedIndex) {
  ImGui::PushID(aName);
  bool returnValue = false;

  const char *preview_item = someItems[aSelectedIndex];
  if (ImGui::BeginCombo(aName, preview_item)) {
    static ImGuiTextFilter filter;
    if (ImGui::IsWindowAppearing()) {
      ImGui::SetKeyboardFocusHere();
      filter.Clear();
    }
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
    filter.Draw("##Filter", -FLT_MIN);

    for (int n = 0; n < aCount; n++) {
      const bool is_selected = (aSelectedIndex == n);
      if (filter.PassFilter(someItems[n]))
        if (ImGui::Selectable(someItems[n], is_selected)) {
          returnValue = true;
          aSelectedIndex = n;
        }
    }
    ImGui::EndCombo();
  }

  ImGui::PopID();
  return returnValue;
}

bool ImGuiHelpers::DrawCombo(const char *aName, const char *someItems[],
                             int aCount, int &aSelectedIndex) {
  ImGui::PushID(aName);
  bool returnValue = false;

  const char *preview_item = someItems[aSelectedIndex];
  if (ImGui::BeginCombo(aName, preview_item)) {
    for (int n = 0; n < aCount; n++) {
      const bool is_selected = (aSelectedIndex == n);
      if (ImGui::Selectable(someItems[n], is_selected)) {
        returnValue = true;
        aSelectedIndex = n;
      }
    }
    ImGui::EndCombo();
  }

  ImGui::PopID();
  return returnValue;
}
