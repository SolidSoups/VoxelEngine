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
    aScene.toggleWireframe = !aScene.toggleWireframe;
  }
  // rearm
  if(glfwGetKey(aWindow, GLFW_KEY_G) == GLFW_RELEASE and
     prevButtonState){
    prevButtonState = false;
  }
}

void InputLayer::ControlCamera(GLFWwindow *aWindow, CameraController &aCamera) {
  // check for reset button
  if (glfwGetKey(aWindow, GLFW_KEY_R) == GLFW_PRESS) {
    aCamera.Reset();
  }

  // get mouse delta
  glm::dvec2 mousePosition;
  glfwGetCursorPos(aWindow, &mousePosition.x, &mousePosition.y);
  glm::vec2 mouseDelta = glm::vec2(mousePosition) - myPrevMousePosition;

  // get the input state
  bool lmb = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_LEFT);
  bool mmb = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_MIDDLE);
  bool rmb = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_RIGHT);
  bool shift = glfwGetKey(aWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
  bool ctrl = glfwGetKey(aWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
  bool alt = glfwGetKey(aWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;

  bool panAction = mmb || (shift && !alt);
  bool orbitAction = (rmb && shift) || (alt && !shift);
  bool zoomDragAction = ctrl && lmb;

  bool navigating = panAction || orbitAction || zoomDragAction;
  if (navigating && !myWasNavigating)
    glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  else if (!navigating && myWasNavigating)
    glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  myWasNavigating = navigating;

  if (panAction) {
    aCamera.Pan(mouseDelta);
  } else if (orbitAction) {
    aCamera.Orbit(mouseDelta);
  } else if (zoomDragAction) {
    aCamera.ZoomDrag(mouseDelta.y);
  }

  // ALWAYS ZOOM
  aCamera.Zoom(ourScrollAccum);

  // reset the scroll accumulation, track prev mouse pos
  ourScrollAccum = 0.0f;
  myPrevMousePosition = mousePosition;

  myDebug.mousePos = mousePosition;
  myDebug.delta = mouseDelta;
  myDebug.lmb = lmb;
  myDebug.mmb = mmb;
  myDebug.rmb = rmb;
  myDebug.shift = shift;
  myDebug.ctrl = ctrl;
  myDebug.pan = panAction;
  myDebug.orbit = orbitAction;
  myDebug.zoomDrag = zoomDragAction;
}
