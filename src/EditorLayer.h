#pragma once

struct GLFWwindow;

class EditorLayer{
public:
  static void Initialize(GLFWwindow* aWindow);
  static void BeginFrame();
  static void EndFrame();
  static void Shutdown();
};
