#pragma once

class PhysicsEngine;
class Scene;

enum RuntimeMode : uint8_t {
  RuntimeMode_STOPPED = 0,
  RuntimeMode_PAUSED = 1,
  RuntimeMode_RUNNING = 2,

  RuntimeMode_MAX
};

class PhysicsScheduler {
public:
  PhysicsScheduler(Scene &aScene);
  ~PhysicsScheduler();
  void Update(float aDeltaTime);
  inline void Reset() {
    myTimeAccumulator = 0.0f;
    myStepClock = 0;
  }
  inline RuntimeMode GetRuntimeMode() const {
    return myRuntimeMode;
  }
  inline int GetStepCount() const {
    return myStepClock;
  }
  inline void SetRuntimeMode(RuntimeMode aRuntimeMode) {
    myRuntimeMode = aRuntimeMode;
  }
  inline void StepForward() {
    myCanRunNextStep = true;
  }
  inline float GetAccumulatedTickTime() const {
    return myTimeAccumulator;
  }
  inline float GetTotalTickLength(){
    return 1.0f / stepsPerSecond;
  }
  inline PhysicsEngine& GetEngine() { return *myEngine; }
  int stepsPerSecond = 16;
private:

  void RunAuto(float aDeltaTime);
  void RunStepped();
  void SimulateOneStep();

private:
  PhysicsEngine* myEngine; 
  RuntimeMode myRuntimeMode = RuntimeMode_STOPPED;
  bool myCanRunNextStep = false;

  double myTimeAccumulator = 0.0f;
  uint32_t myStepClock = 0;
};
