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
#include "objects/Framebuffer.h"

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
  EditorLayer::AddStatsEditor(myScene, physicsRuntime);

  VoxelPainter::SetCurrentChunk(&myScene.GetVoxelChunk());
  VoxelPainter painter;
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

    // Control camera movement
    inputLayer.ControlCamera(window, myCameraController);
    myCameraController.MoveCamera(myCamera, deltaTime);
    inputLayer.ControlScene(window, myScene);



    // Begin frame
    EditorLayer::BeginFrame();
    glm::ivec2 viewportSize = EditorLayer::GetViewportSize();
    myCamera.aspectRatio = (float)viewportSize.x / viewportSize.y;
    Renderer::BeginFrame(myCamera, viewportSize);

    // painter needs fresh aspect ratio and viewport sizes
    inputLayer.ControlPainter(window, myCamera, myScene, painter);

    // Render
    Renderer::RenderFrame(myCamera, myScene, viewportSize);
    Renderer::DrawVoxelGrid();

    // end frame
    Renderer::EndFrame();
    EditorLayer::EndFrame();
    ApplicationLayer::SwapBuffers();
  }

  // Destroy EVERYTHING!
  Renderer::Destroy();
  EditorLayer::Shutdown();
  ApplicationLayer::Destroy();
  return 0;
}
