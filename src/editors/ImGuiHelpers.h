#pragma once

class ImGuiHelpers{
public:
  static bool DrawFilterCombo(const char* aName, const char* someItems[], int aCount, int& aSelectedIndex);
  static bool DrawCombo(const char* aName, const char* someItems[], int aCount, int& aSelectedIndex);
};
