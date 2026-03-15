#include <iostream>
#include <print>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create a window
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Voxel Playground", NULL, NULL);
  if (window == NULL) {
    std::print("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Load glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::print(std::cerr, "Failed to initialize GLAD");
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

  glViewport(0, 0, 800, 600);

  while(!glfwWindowShouldClose(window)){
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  std::print("Hello C++23 World!");

  glfwTerminate();
  return 0;
}
