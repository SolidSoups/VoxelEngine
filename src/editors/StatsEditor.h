#pragma once

#include "Editor.h"

class Scene;
class PhysicsRuntime;

class StatsEditor : public Editor {
public:
  StatsEditor(Scene &aScene, PhysicsRuntime& aRuntime);
  StatsEditor(const StatsEditor &) = delete;
  StatsEditor &operator=(const StatsEditor &) = delete;
  StatsEditor(StatsEditor &&other) noexcept = default;

  void Draw() override;
  void CollectStats();

private:
  std::array<uint32_t, 1000> myStoneTriCounts;
  std::array<uint32_t, 1000> mySandTriCounts;
  std::array<uint32_t, 1000> myWaterTriCounts;
  std::array<uint32_t, 1000> myStoneVoxelCounts;
  std::array<uint32_t, 1000> mySandVoxelCounts;
  std::array<uint32_t, 1000> myWaterVoxelCounts;
  std::array<uint32_t, 1000> myMemoryCounts;
  std::array<uint32_t, 1000> myTimepoints;
  std::array<double, 1000> myDoubleTimepoints;
  std::array<double, 1000> meshTimings;
  int myFrameCount = 0;

  Scene &myScene;
  PhysicsRuntime &myPhysicsRuntime;
};
