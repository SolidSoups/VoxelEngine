#include "ApplicationLayer.h"
#include "EditorLayer.h"
#include "Renderer.h"
#include "Scene.h"
#include "InputLayer.h"
#include "CameraController.h"
#include "VoxelPainter.h"
#include "objects/Camera.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "Voxel Playground"

int main() {

  if(!ApplicationLayer::Initialize(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)){
    return -1;
  }
  EditorLayer::Initialize(ApplicationLayer::GetWindow());
  Renderer::Initialize();

  
  auto* window = ApplicationLayer::GetWindow();

  InputLayer inputLayer;
  inputLayer.Initialize(window);
  Scene myScene;
  CameraController myCameraController;
  Camera myCamera;

  VoxelPainter::SetCurrentChunk(&myScene.GetVoxelChunk());

  float deltaTime = 0.f, lastTime = 0.f;
  while (!ApplicationLayer::ShouldClose()) {
    ApplicationLayer::PollEvents();

    inputLayer.UpdateInput(window);

    // calculate delta time
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // update camera aspect ratio
    myCamera.aspectRatio = ApplicationLayer::GetAspectRatio();

    // control camera
    inputLayer.ControlCamera(window, myCameraController);
    myCameraController.MoveCamera(myCamera, deltaTime);

    // draw frame
    EditorLayer::BeginFrame();
    Renderer::BeginFrame(myCamera);
    myScene.Render();
    Renderer::DrawVoxelGrid();

    // end frame
    EditorLayer::EndFrame();
    ApplicationLayer::SwapBuffers();
  }

  // Destroy EVERYTHING!
  Renderer::Destroy();
  EditorLayer::Shutdown();
  ApplicationLayer::Destroy();
  return 0;
}
