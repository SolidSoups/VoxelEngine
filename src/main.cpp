#include <imgui.h>
#include "ApplicationLayer.h"
#include "EditorLayer.h"
#include "Renderer.h"
#include "Scene.h"
#include "InputLayer.h"
#include "PhysicsRuntime.h"
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
  PhysicsRuntime physicsRuntime(myScene);

  EditorLayer::AddRuntimeEditor(&physicsRuntime);

  VoxelPainter::SetCurrentChunk(&myScene.GetVoxelChunk());
  Renderer::SetWireframeMode(true);

  float deltaTime = 0.f, lastTime = 0.f;
  while (!ApplicationLayer::ShouldClose()) {
    ApplicationLayer::PollEvents();
    inputLayer.UpdateInput(window);

    // Calculate delta time
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Do a physics step
    physicsRuntime.Update(deltaTime);

    // update scene, remesh chunk if needed
    myScene.Update();

    // Update camera with new aspect ratio
    myCamera.aspectRatio = ApplicationLayer::GetAspectRatio();

    // Control camera movement
    inputLayer.ControlCamera(window, myCameraController);
    myCameraController.MoveCamera(myCamera, deltaTime);
    inputLayer.ControlScene(window, myScene);


    // Begin frame
    EditorLayer::BeginFrame();
    Renderer::BeginFrame(myCamera);

    // Render
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
