#include "StatsEditor.h"

#include <algorithm>
#include <imgui.h>
#include <implot.h>

#include "../objects/VoxelType.h"
#include "../Scene.h"
#include "../PhysicsRuntime.h"

// function exists, but only in the internal translation unit
// forward declare it so we can use it
namespace ImPlot {
bool IsItemHidden(const char *label_id);
}

StatsEditor::StatsEditor(Scene &aScene, PhysicsRuntime &aRuntime)
    : myScene(aScene), myPhysicsRuntime(aRuntime) {}

void StatsEditor::Draw() {
  static bool isTriStoneHidden = false;
  static bool isTriSandHidden = false;
  static bool isTriWaterHidden = false;
  static bool isVoxStoneHidden = false;
  static bool isVoxSandHidden = false;
  static bool isVoxWaterHidden = false;

  ImGui::Begin("Statistics");

  int count = std::min((int)myFrameCount, (int)1000);
  int offset = myFrameCount >= 1000 ? (myFrameCount % 1000) : 0;
  size_t currentIndex = (myFrameCount > 0 ? myFrameCount - 1 : 0) % 1000;

  // Voxel count plot
  // ensure y axis always has 1/5th space on top
  uint32_t voxelMaxVal = 0;
  if (!isVoxStoneHidden)
    voxelMaxVal =
        std::max(voxelMaxVal, *std::max_element(myStoneVoxelCounts.begin(),
                                                myStoneVoxelCounts.end()));
  if (!isVoxSandHidden)
    voxelMaxVal =
        std::max(voxelMaxVal, *std::max_element(mySandVoxelCounts.begin(),
                                                mySandVoxelCounts.end()));
  if (!isVoxWaterHidden)
    voxelMaxVal =
        std::max(voxelMaxVal, *std::max_element(myWaterVoxelCounts.begin(),
                                                myWaterVoxelCounts.end()));
  ImPlot::SetNextAxesLimits(myFrameCount >= 1000 ? myFrameCount - 1000 : 0,
                            myFrameCount, 0, voxelMaxVal * 1.25,
                            ImGuiCond_Always);
  if (ImPlot::BeginPlot("Voxel Count")) {
    ImPlot::SetNextLineStyle(ImVec4(0.5, 0.5, 0.5, 1.0));
    ImPlot::PlotLine("STONE", myTimepoints.data(), mySandVoxelCounts.data(),
                     count, 0, offset, sizeof(uint32_t));
    isVoxStoneHidden = ImPlot::IsItemHidden("STONE");
    if (!isVoxStoneHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         myStoneVoxelCounts[currentIndex],
                         ImVec4(0.5, 0.5, 0.5, 1), ImVec2(10, 10), true, "%u",
                         myStoneVoxelCounts[currentIndex]);

    ImPlot::SetNextLineStyle(ImVec4(1.0, 1.0, 0, 1));
    ImPlot::PlotLine("SAND", myTimepoints.data(), mySandVoxelCounts.data(),
                     count, 0, offset, sizeof(uint32_t));
    isVoxStoneHidden = ImPlot::IsItemHidden("SAND");
    if (!isVoxSandHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         mySandVoxelCounts[currentIndex],
                         ImVec4(1.0, 1.0, 0, 1), ImVec2(10, 10), true, "%u",
                         mySandVoxelCounts[currentIndex]);

    ImPlot::SetNextLineStyle(ImVec4(0, 0.588, 1.0, 1));
    ImPlot::PlotLine("WATER", myTimepoints.data(), myWaterVoxelCounts.data(),
                     count, 0, offset, sizeof(uint32_t));
    isVoxWaterHidden = ImPlot::IsItemHidden("WATER");
    if (!isVoxWaterHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         myWaterVoxelCounts[currentIndex],
                         ImVec4(0, 0.588, 1.0, 1), ImVec2(10, 10), true, "%u",
                         myWaterVoxelCounts[currentIndex]);

    ImPlot::EndPlot();
  }

  // Triangle count plot

  // ensure y axis always haves 1/5th space
  uint32_t triangleMaxVal = 0;
  uint32_t triangleMinVal = UINT32_MAX;
  auto stoneBegin = myStoneTriCounts.begin();
  auto sandBegin = mySandTriCounts.begin();
  auto waterBegin = myWaterTriCounts.begin();
  if (!isTriStoneHidden) {
    triangleMaxVal = std::max(
        triangleMaxVal, *std::max_element(stoneBegin, stoneBegin + count));
    triangleMinVal = std::min(
        triangleMinVal, *std::min_element(stoneBegin, stoneBegin + count));
  }
  if (!isTriSandHidden) {
    triangleMaxVal =
        std::max(triangleMaxVal, *std::max_element(sandBegin,
                                                   sandBegin + count));
    triangleMinVal =
        std::min(triangleMinVal, *std::min_element(sandBegin,
                                                   sandBegin + count));
  }
  if (!isTriWaterHidden) {
    triangleMaxVal =
        std::max(triangleMaxVal, *std::max_element(waterBegin,
                                                   waterBegin + count));
    triangleMinVal =
        std::min(triangleMinVal, *std::min_element(waterBegin,
                                                   waterBegin + count));
  }

  ImPlot::SetNextAxesLimits(myFrameCount >= 1000 ? myFrameCount - 1000 : 0,
                            myFrameCount, triangleMinVal * 0.80f,
                            triangleMaxVal * 1.25f, ImGuiCond_Always);
  if (ImPlot::BeginPlot("Triangle Count")) {
    // plot stone triangle count
    ImPlot::SetNextLineStyle(ImVec4(0.5, 0.5, 0.5, 1));
    ImPlot::PlotLine("STONE", myTimepoints.data(), myStoneTriCounts.data(),
                     count, 0, offset, sizeof(uint32_t));
    isTriStoneHidden = ImPlot::IsItemHidden("STONE");
    if (!isTriStoneHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         myStoneTriCounts[currentIndex],
                         ImVec4(0.5, 0.5, 0.5, 1), ImVec2(10, 10), true, "%u",
                         myStoneTriCounts[currentIndex]);

    // plot sand triangle count
    ImPlot::SetNextLineStyle(ImVec4(1.0, 1.0, 0.0, 1.0));
    ImPlot::PlotLine("SAND", myTimepoints.data(), mySandTriCounts.data(), count,
                     0, offset, sizeof(uint32_t));
    isTriSandHidden = ImPlot::IsItemHidden("SAND");
    if (!isTriSandHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         mySandTriCounts[currentIndex], ImVec4(1.0, 1.0, 0, 1),
                         ImVec2(10, 10), true, "%u",
                         mySandTriCounts[currentIndex]);

    // plot water triangle count
    ImPlot::SetNextLineStyle(ImVec4(0, 0.588, 1, 1));
    ImPlot::PlotLine("WATER", myTimepoints.data(), myWaterTriCounts.data(),
                     count, 0, offset, sizeof(uint32_t));
    isTriWaterHidden = ImPlot::IsItemHidden("WATER");
    if (!isTriWaterHidden)
      ImPlot::Annotation(myTimepoints[currentIndex],
                         myWaterTriCounts[currentIndex], ImVec4(0, 0.588, 1, 1),
                         ImVec2(10, 10), true, "%u",
                         myWaterTriCounts[currentIndex]);

    ImPlot::EndPlot();
  }

  // Mesh timings plot
  auto maxVal =
      std::max(1.0, *std::max_element(meshTimings.begin(), meshTimings.end()));
  ImPlot::SetNextAxesLimits(myFrameCount >= 1000 ? myFrameCount - 1000 : 0,
                            myFrameCount, 0, maxVal * 1.25f, ImGuiCond_Always);
  if (ImPlot::BeginPlot("Mesh Build Time")) {
    ImPlot::PlotLine("Build Time", myDoubleTimepoints.data(),
                     meshTimings.data(), count, 0, offset, sizeof(double));
    ImPlot::Annotation(myDoubleTimepoints[currentIndex],
                       meshTimings[currentIndex], ImVec4(1, 1, 1, 1),
                       ImVec2(10, 10), true, "%.2fms",
                       meshTimings[currentIndex]);
    ImPlot::EndPlot();
  }

  ImGui::End();
}

void StatsEditor::CollectStats() {
  static bool firstFrame = true;
  if (myPhysicsRuntime.GetRuntimeMode() != RuntimeMode_RUNNING && !firstFrame) {
    return;
  }
  if (firstFrame)
    firstFrame = false;

  auto latestStats = myScene.GetStats();
  auto &chunk = myScene.GetVoxelChunk();
  size_t currentIndex = myFrameCount % 1000;
  myStoneTriCounts[currentIndex] = latestStats.stoneFaceCount;
  mySandTriCounts[currentIndex] = latestStats.sandFaceCount;
  myWaterTriCounts[currentIndex] = latestStats.waterFaceCount;
  myStoneVoxelCounts[currentIndex] = chunk.CountVoxelType(VoxelType_STONE);
  mySandVoxelCounts[currentIndex] = chunk.CountVoxelType(VoxelType_SAND);
  myWaterVoxelCounts[currentIndex] = chunk.CountVoxelType(VoxelType_WATER);
  myMemoryCounts[currentIndex] = latestStats.meshMemory;
  meshTimings[currentIndex] = latestStats.getMsTiming();
  myTimepoints[currentIndex] = myFrameCount;
  myDoubleTimepoints[currentIndex] = myFrameCount;
  myFrameCount++;
}
