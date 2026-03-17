#pragma once

#include <string>

struct GLFWwindow;

class ApplicationLayer {
public:
  static bool Initialize(const std::string& aWindowTitle, int aWindowWidth,
                         int aWindowHeight);
  static void Resize(int aNewWindowWidth, int aNewWindowHeight);
  static bool ShouldClose();
  static void SwapBuffers();
  static void CloseApplication();
  static void Destroy();
  static GLFWwindow* GetWindow();
private:
  static std::string myWindowTitle; 
  static int myWindowHeight, myWindowWidth;
  static GLFWwindow* myWindow;
  static bool myShouldClose;
};
