#pragma once

#include <memory>

struct GLFWwindow;
class KeybindsEditor;
class BrushEditor;

class EditorLayer{
public:
  static void Initialize(GLFWwindow* aWindow);
  static void BeginFrame();
  static void EndFrame();
  static void Shutdown();

private:
  static std::unique_ptr<KeybindsEditor> myKeybindsEditor;
  static std::unique_ptr<BrushEditor> myBrushEditor;
};
