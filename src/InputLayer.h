#pragma once

class CameraController;
struct GFLWwindow;

class InputLayer {
public:
  void Initialize(GLFWwindow* aWindow);
  void ControlCamera(GLFWwindow* aWindow, CameraController& aCamera);

private:
  glm::vec2 myPrevMousePosition{0.0f};
};
