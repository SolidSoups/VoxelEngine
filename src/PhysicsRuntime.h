#pragma once

class PhysicsEngine;
class Scene;

enum RuntimeMode : uint8_t {
  RuntimeMode_STOPPED = 0,
  RuntimeMode_PAUSED = 1,
  RuntimeMode_RUNNING = 2,

  RuntimeMode_MAX
};

class PhysicsRuntime {
public:
  PhysicsRuntime(Scene &aScene);
  ~PhysicsRuntime();
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
  int stepsPerSecond = 1;
private:

  void RunAuto(float aDeltaTime);
  void RunStepped();
  void SimulateOneStep();

private:
  PhysicsEngine* myEngine; 
  RuntimeMode myRuntimeMode = RuntimeMode_RUNNING;
  bool myCanRunNextStep = false;

  double myTimeAccumulator = 0.0f;
  uint32_t myStepClock = 0;
};
