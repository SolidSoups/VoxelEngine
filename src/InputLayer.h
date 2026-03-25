#pragma once

class CameraController;
struct GFLWwindow;
class Scene;

class InputLayer {
public:
  void Initialize(GLFWwindow *aWindow);
  void ControlCamera(GLFWwindow *aWindow, CameraController &aCamera);
  void UpdateInput(GLFWwindow *aWindow);
  void ControlScene(GLFWwindow* aWindow, Scene &aScene);

private:
  glm::vec2 myPrevMousePosition{0.0f};
};
