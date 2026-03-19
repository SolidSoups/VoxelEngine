#include "InputLayer.h"

#include <print>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "CameraController.h"

double ourScrollAccum = 0.0f;
void scroll_callback(GLFWwindow* aWindow, double aXOffset, double aYOffset){
  ourScrollAccum += aYOffset;
}

void InputLayer::Initialize(GLFWwindow* aWindow){
  glfwSetScrollCallback(aWindow, scroll_callback);
}

void InputLayer::ControlCamera(GLFWwindow* aWindow, CameraController& aCamera){
  // check for reset button
  if(glfwGetKey(aWindow, GLFW_KEY_F) == GLFW_PRESS){
    aCamera.Reset();
  } 
 
  // get mouse delta
  glm::dvec2 mousePosition;
  glfwGetCursorPos(aWindow, &mousePosition.x, &mousePosition.y);
  glm::vec2 mouseDelta = glm::vec2(mousePosition) - myPrevMousePosition;

  // get the input state
  bool panButton = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_MIDDLE);
  bool orbitButton = glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_RIGHT);

  // PAN!
  if(panButton){
    aCamera.Pan(mouseDelta);
  }
  // ORBIT
  else if(orbitButton){
    aCamera.Orbit(mouseDelta);
  }

  // ALWAYS ZOOM
  aCamera.Zoom(ourScrollAccum); 

  // reset the scroll accumulation, track prev mouse pos
  ourScrollAccum = 0.0f;
  myPrevMousePosition = mousePosition;
}

