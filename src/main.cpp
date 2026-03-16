#include <iostream>
#include <print>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ApplicationLayer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Voxel Playground"

int main() {
  if(!ApplicationLayer::Initialize(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)){
    return -1;
  }

  while (!ApplicationLayer::ShouldClose()) {
    glfwPollEvents();

    ApplicationLayer::SwapBuffers();
  }

  ApplicationLayer::Destroy();
  return 0;
}
