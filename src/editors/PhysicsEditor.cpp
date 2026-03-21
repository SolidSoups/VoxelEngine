#include "PhysicsEditor.h"

#include <string>

#include <imgui.h>
#include <glm/glm.hpp>

#include "../PhysicsRuntime.h"

void PhysicsEditor::Draw(){
  ImGui::Begin("Runtime");

  // display error if we can't run this widget
  if(!myRuntime){
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 0.0f), "ERROR: PhysicsRuntime not injected.");
    return;
  }

  RuntimeMode currentMode = myRuntime->GetRuntimeMode();
  bool isRunning = 
    currentMode == RuntimeMode_RUNNING or
    currentMode == RuntimeMode_PAUSED;


  // Run simulation buttons
  ImGui::BeginDisabled(isRunning);
  if(ImGui::Button("Run")) {
    // run
    myRuntime->SetRuntimeMode(RuntimeMode_RUNNING);
  }
  ImGui::SameLine();

  if(ImGui::Button("Run Paused")){
    // run and immediately pause
    myRuntime->SetRuntimeMode(RuntimeMode_PAUSED);
  }
  ImGui::EndDisabled();

  // Buttons to control simulation
  ImGui::BeginDisabled(!isRunning);
  if(ImGui::Button("Stop")){
    // stop simulation
    myRuntime->SetRuntimeMode(RuntimeMode_STOPPED);
  }
  ImGui::SameLine();
  if(ImGui::Button("Resume AUTO")){
    // resume simulation
    myRuntime->SetRuntimeMode(RuntimeMode_RUNNING);
  }
  ImGui::SameLine();
  if(ImGui::Button("Pause")){
    // pause simulation
    myRuntime->SetRuntimeMode(RuntimeMode_PAUSED);
  }
  ImGui::SameLine();
  if(ImGui::Button("Step")){
    // perform one step of the simulation
    myRuntime->StepForward();
  }
  ImGui::EndDisabled();
  ImGui::Dummy(ImVec2(0, 5));

  // Let user adjust step speed
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Steps per second");
  ImGui::SameLine();
  if(ImGui::InputInt("##StepsPerSecond", &myRuntime->stepsPerSecond, 1, 0, 32))
    myRuntime->stepsPerSecond = glm::clamp(myRuntime->stepsPerSecond, 1, 32);

  // Show step count
  int stepCount = myRuntime->GetStepCount();
  std::string stepText = "Step: " + std::to_string(stepCount);
  ImGui::Text(stepText.c_str());



  ImGui::End();
}

