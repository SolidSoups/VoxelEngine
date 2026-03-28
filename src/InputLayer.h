#pragma once

class CameraController;
struct GFLWwindow;
class Scene;

struct InputDebug {
  glm::dvec2 mousePos{};
  glm::vec2 delta{};
  bool lmb{}, mmb{}, rmb{}, shift{}, ctrl{};
  bool pan{}, orbit{}, zoomDrag{};
};

class InputLayer {
public:
  void Initialize(GLFWwindow *aWindow);
  void ControlCamera(GLFWwindow *aWindow, CameraController &aCamera);
  void UpdateInput(GLFWwindow *aWindow);
  void ControlScene(GLFWwindow* aWindow, Scene &aScene);

  InputDebug myDebug;

private:
  glm::vec2 myPrevMousePosition{0.0f};
  bool myWasNavigating{false};
};
