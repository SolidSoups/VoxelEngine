#include "editors/PhysicsEditor.h"

#include <string>
#include <cstdio>

#include <imgui.h>
#include <glm/glm.hpp>

#include "rendering/Texture.h"
#include "physics/PhysicsRuntime.h"

#define PLAY_TEX_PATH "./res/play.png"
#define PAUSE_TEX_PATH "./res/pause.png"
#define PLAYPAUSED_TEX_PATH "./res/play_paused.png"
#define STOP_TEX_PATH "./res/stop.png"
#define STEP_TEX_PATH "./res/step.png"

#define BUTTON_SIZE ImVec2(32, 32)

PhysicsEditor::PhysicsEditor(PhysicsRuntime *aRuntime) : myRuntime(aRuntime) {
  // customize the textures
  TextureConfig config;
  config.flipY = false;
  config.minFilter = GL_NEAREST;
  config.magFilter = GL_NEAREST;

  // load textures
  myPlayTexture = std::make_unique<Texture>(PLAY_TEX_PATH);
  myPauseTexture = std::make_unique<Texture>(PAUSE_TEX_PATH);
  myPlayPausedTexture = std::make_unique<Texture>(PLAYPAUSED_TEX_PATH);
  myStopTexture = std::make_unique<Texture>(STOP_TEX_PATH);
  myStepTexture = std::make_unique<Texture>(STEP_TEX_PATH);
}

PhysicsEditor::~PhysicsEditor() {}

#define BAR_CHART_COLOR ImVec4(0.2f, 0.65, 0.0f, 1.0f)
void PhysicsEditor::Draw() {
  ImGui::Begin("Runtime");

  // display error if we can't run this widget
  if (!myRuntime) {
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 0.0f),
                       "ERROR: PhysicsRuntime not injected.");
    return;
  }

  RuntimeMode currentMode = myRuntime->GetRuntimeMode();

  float buttonCount = (currentMode == RuntimeMode_STOPPED) ? 2.0f : 3.0f;
  float totalWidth = buttonCount * BUTTON_SIZE.x +
                     (buttonCount - 1) * ImGui::GetStyle().ItemSpacing.x +
                     ImGui::GetStyle().FramePadding.x * 2 * buttonCount;
  float totalHeight = BUTTON_SIZE.y + ImGui::GetStyle().FramePadding.y * 2;

  float availWidth = ImGui::GetContentRegionAvail().x;
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       (availWidth - totalWidth) * 0.5f);

  ImGui::BeginChild("##RuntimeButtons", ImVec2(totalWidth, totalHeight),
                    ImGuiChildFlags_None);
  if (currentMode == RuntimeMode_STOPPED) {
    DrawButtonsNotRunning();
  } else {
    DrawButtonsRunning(currentMode == RuntimeMode_PAUSED);
  }
  ImGui::EndChild();

  ImGui::Dummy(ImVec2(0, 5));

  // // get times and a formatted string
  // float tickTime = myRuntime->GetAccumulatedTickTime();
  // float totalTick = myRuntime->GetTotalTickLength();
  // char buf[64];
  // snprintf(buf, sizeof(buf), "%.2f / %.2f", tickTime, totalTick);
  //
  // // show progress bar
  // ImGui::PushStyleColor(ImGuiCol_PlotHistogram, BAR_CHART_COLOR);
  // ImGui::Text("Tick Progression");
  // ImGui::ProgressBar(tickTime / totalTick, ImVec2(-1.f, 0.f), "");
  // ImGui::PopStyleColor();
  //
  // // Draw centered text on top
  // ImVec2 barMin = ImGui::GetItemRectMin();
  // ImVec2 barMax = ImGui::GetItemRectMax();
  // ImVec2 textSize = ImGui::CalcTextSize(buf);
  // ImVec2 textPos = ImVec2(barMin.x + (barMax.x - barMin.x - textSize.x) * 0.5f,
  //                         barMin.y + (barMax.y - barMin.y - textSize.y) * 0.5f);
  // ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255),
  //                                     buf);
  //
  // ImGui::Dummy(ImVec2(0, 5));

  // Let user adjust step speed
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Steps per second");
  ImGui::SameLine();
  if (ImGui::InputInt("##StepsPerSecond", &myRuntime->stepsPerSecond, 1, 0, 32))
    myRuntime->stepsPerSecond = glm::clamp(myRuntime->stepsPerSecond, 1, 32);

  // Show step count
  int stepCount = myRuntime->GetStepCount();
  std::string stepText = "Step: " + std::to_string(stepCount);
  ImGui::Text(stepText.c_str());

  ImGui::End();
}

void PhysicsEditor::DrawButtonsNotRunning() {
  if (ImGui::ImageButton("#RunButton",
                         (ImTextureRef)(intptr_t)myPlayTexture->id,
                         BUTTON_SIZE)) {
    myRuntime->SetRuntimeMode(RuntimeMode_RUNNING);
  }
  ImGui::SameLine();
  if (ImGui::ImageButton("#PlayPausedButton",
                         (ImTextureRef)(intptr_t)myPlayPausedTexture->id,
                         BUTTON_SIZE)) {
    myRuntime->SetRuntimeMode(RuntimeMode_PAUSED);
  }
}
void PhysicsEditor::DrawButtonsRunning(bool isPaused) {
  if (ImGui::ImageButton("#StopButton",
                         (ImTextureRef)(intptr_t)myStopTexture->id,
                         BUTTON_SIZE)) {
    // stop simulation
    myRuntime->SetRuntimeMode(RuntimeMode_STOPPED);
  }

  // Switch been play and pause button
  ImGui::SameLine();
  if (isPaused) {
    if (ImGui::ImageButton("#PlayButton",
                           (ImTextureRef)(intptr_t)myPlayTexture->id,
                           BUTTON_SIZE)) {
      myRuntime->SetRuntimeMode(RuntimeMode_RUNNING);
    }
  } else {
    if (ImGui::ImageButton("#PauseButton",
                           (ImTextureRef)(intptr_t)myPauseTexture->id,
                           BUTTON_SIZE)) {
      // pause simulation
      myRuntime->SetRuntimeMode(RuntimeMode_PAUSED);
    }
  }

  // Draw step button if we are paused
  ImGui::BeginDisabled(!isPaused);
  ImGui::SameLine();
  if (ImGui::ImageButton("#StepButton",
                         (ImTextureRef)(intptr_t)myStepTexture->id,
                         BUTTON_SIZE)) {
    // perform one step of the simulation
    myRuntime->StepForward();
  }
  ImGui::EndDisabled();
}
