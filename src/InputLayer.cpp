#include "InputLayer.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

#include "Scene.h"
#include "CameraController.h"
#include "ApplicationLayer.h"

double ourScrollAccum = 0.0f;
void scroll_callback(GLFWwindow *aWindow, double aXOffset, double aYOffset) {
  ImGui_ImplGlfw_ScrollCallback(aWindow, aXOffset, aYOffset);
  ourScrollAccum += aYOffset;
}

void InputLayer::Initialize(GLFWwindow *aWindow) {
  glfwSetScrollCallback(aWindow, scroll_callback);
}

void InputLayer::UpdateInput(GLFWwindow *aWindow) {
  // escape to close application
  if (glfwGetKey(aWindow, GLFW_KEY_ESCAPE)) {
    ApplicationLayer::CloseApplication();
  }
}

void InputLayer::ControlScene(GLFWwindow *aWindow, Scene &aScene) {
  static bool prevButtonState = false;
  // engage
  if (glfwGetKey(aWindow, GLFW_KEY_G) == GLFW_PRESS and
      !prevButtonState) {
    prevButtonState = true;
    aScene.toggleMesh = !aScene.toggleMesh;
  }
  // rearm
  if(glfwGetKey(aWindow, GLFW_KEY_G) == GLFW_RELEASE and
     prevButtonState){
    prevButtonState = false;
  }
}

void InputLayer::ControlCamera(GLFWwindow *aWindow, CameraController &aCamera) {
  if (ImGui::GetIO().WantCaptureMouse)
    return;

  // check for reset button
  if (glfwGetKey(aWindow, GLFW_KEY_R) == GLFW_PRESS) {
    aCamera.Reset();
  }

  // get mouse delta
  glm::dvec2 mousePosition;
  glfwGetCursorPos(aWindow, &mousePosition.x, &mousePosition.y);
  glm::vec2 mouseDelta = glm::vec2(mousePosition) - myPrevMousePosition;

  // get the input state
  bool panButton = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_MIDDLE);
  bool orbitButton = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_RIGHT) &&
                     glfwGetKey(aWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  // PAN!
  if (panButton) {
    aCamera.Pan(mouseDelta);
  }
  // ORBIT
  else if (orbitButton) {
    aCamera.Orbit(mouseDelta);
  }

  // ALWAYS ZOOM
  aCamera.Zoom(ourScrollAccum);

  // reset the scroll accumulation, track prev mouse pos
  ourScrollAccum = 0.0f;
  myPrevMousePosition = mousePosition;
}
