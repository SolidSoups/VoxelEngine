#pragma once

#include <vector>
#include <memory>

struct GLFWwindow;
class KeybindsEditor;
class BrushEditor;
class PhysicsRuntime;
class ViewportEditor;
class Editor;

class EditorLayer{
public:
  static void Initialize(GLFWwindow* aWindow);
  static void BeginFrame();
  static void EndFrame();
  static void Shutdown();

public:
  static void AddRuntimeEditor(PhysicsRuntime* aPhysicsRuntime);

private:
  static std::unique_ptr<KeybindsEditor> myKeybindsEditor;
  static std::unique_ptr<BrushEditor> myBrushEditor;
  static std::unique_ptr<ViewportEditor> myViewportEditor;
  static std::vector<std::unique_ptr<Editor>> myEditors;
  static unsigned int myMainDockspace;
};
