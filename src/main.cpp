#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ApplicationLayer.h"
#include "EditorLayer.h"
#include "Renderer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Voxel Playground"

int main() {
  if(!ApplicationLayer::Initialize(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)){
    return -1;
  }

  EditorLayer::Initialize(ApplicationLayer::GetWindow());

  Renderer::Initialize();

  auto* window = ApplicationLayer::GetWindow();
  while (!ApplicationLayer::ShouldClose()) {
    glfwPollEvents();
    
    // escape to close application
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)){
      ApplicationLayer::CloseApplication(); 
    }

    // clear background
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    EditorLayer::BeginFrame();
    Renderer::DrawCube();

    EditorLayer::EndFrame();
    ApplicationLayer::SwapBuffers();
  }

  Renderer::Destroy();
  EditorLayer::Shutdown();
  ApplicationLayer::Destroy();
  return 0;
}
