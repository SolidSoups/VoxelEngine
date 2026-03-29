#include "ApplicationLayer.h"

#include <print>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"

std::string ApplicationLayer::myWindowTitle = "";
int ApplicationLayer::myWindowHeight = -1;
int ApplicationLayer::myWindowWidth = -1;
GLFWwindow *ApplicationLayer::myWindow = nullptr;
bool ApplicationLayer::myShouldClose = false;

void framebuffer_size_callback(GLFWwindow *aWindow, int aWidth, int aHeight) {
  ApplicationLayer::Resize(aWidth, aHeight);
  Renderer::framebuffer.Resize(aWidth, aHeight);
}

void ApplicationLayer::Resize(int aNewWindowWidth, int aNewWindowHeight) {
  ApplicationLayer::myWindowWidth = aNewWindowWidth;
  ApplicationLayer::myWindowHeight = aNewWindowHeight;
  glViewport(0, 0, ApplicationLayer::myWindowWidth,
             ApplicationLayer::myWindowHeight);
}

bool ApplicationLayer::Initialize(const std::string &aWindowTitle,
                                  int aWindowWidth, int aWindowHeight) {
  ApplicationLayer::myWindowTitle = aWindowTitle;
  ApplicationLayer::myWindowHeight = aWindowHeight;
  ApplicationLayer::myWindowWidth = aWindowWidth;

  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create the window
  ApplicationLayer::myWindow = glfwCreateWindow(
      ApplicationLayer::myWindowWidth, ApplicationLayer::myWindowHeight,
      ApplicationLayer::myWindowTitle.c_str(), NULL, NULL);
  if (ApplicationLayer::myWindow == NULL) {
    std::print("Failed to create GLFW window");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(ApplicationLayer::myWindow);

  // Load glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::print("Failed to initialize GLAD");
    return false;
  }

  glfwSetFramebufferSizeCallback(ApplicationLayer::myWindow,
                                 framebuffer_size_callback);

  glViewport(0, 0, ApplicationLayer::myWindowWidth,
             ApplicationLayer::myWindowHeight);
  return true;
}


void ApplicationLayer::PollEvents(){
  glfwPollEvents();
}

bool ApplicationLayer::ShouldClose() {
  return myShouldClose or glfwWindowShouldClose(ApplicationLayer::myWindow);
}

void ApplicationLayer::SwapBuffers() {
  glfwSwapBuffers(ApplicationLayer::myWindow);
}

void ApplicationLayer::CloseApplication(){
  ApplicationLayer::myShouldClose = true;
}

void ApplicationLayer::Destroy() {
  glfwTerminate();
}
GLFWwindow* ApplicationLayer::GetWindow(){
  return ApplicationLayer::myWindow;
}


glm::vec2 ApplicationLayer::GetWindowSize(){
  return {ApplicationLayer::myWindowWidth, ApplicationLayer::myWindowHeight};
}

float ApplicationLayer::GetAspectRatio(){
  return (float)ApplicationLayer::myWindowWidth / ApplicationLayer::myWindowHeight;
}
