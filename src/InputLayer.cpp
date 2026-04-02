#include "InputLayer.h"

#include <print>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>

#include "Scene.h"
#include "objects/Camera.h"
#include "CameraController.h"
#include "ApplicationLayer.h"
#include "VoxelPainter.h"
#include "editors/EditorIO.h"

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


void InputLayer::ControlPainter(GLFWwindow* aWindow, Camera &aCamera, Scene& aScene, VoxelPainter& aPainter){
  if(!EditorIO::Get().ViewportIsHovered)
    return;

  static bool pressedLastFrame = false;
  
  // deproject screen mouse position to grid space position
  glm::vec3 rayOrigin, rayDir;    
  glm::ivec2 screenSize = EditorIO::Get().ViewportSize;
  auto mousePos = EditorIO::GetViewportMousePosition();
  aCamera.DeprojectMouseToRay(mousePos, screenSize, rayOrigin, rayDir);
  glm::ivec3 position = VoxelPainter::DDARaycastGetPosition(rayOrigin, rayDir, aScene.GetVoxelChunk() );

  // tell scene to display cursor
  auto& paintState = PainterState::Get();
  if(position.x > 0){
    paintState.cursorPosition = (glm::vec3)position * (4.0f / CHUNK_SIZE);
    paintState.drawCursor = true;
  }
  else
    paintState.drawCursor = false;

  // if pressed, place a voxel down
  if(glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS and !pressedLastFrame)
  {
    pressedLastFrame = true;
    if(position.x > 0){
      VoxelPainter::SetBrushColor(VoxelType_SAND);
      VoxelPainter::SetBrushType(BrushType_SPHERE);
      VoxelPainter::EditorPaint(position, {}, 3);
    }
  }

  // reset press
  if(glfwGetMouseButton(aWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    pressedLastFrame = false;
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


