#pragma once

class PhysicsEngine;
class Scene;

enum RuntimeMode : uint8_t {
  RuntimeMode_STOPPED = 0,
  RuntimeMode_PAUSED = 1,
  RuntimeMode_STEPPING = 2,
  RuntimeMode_RUNNING = 3,

  RuntimeMode_MAX
};

class PhysicsRuntime {
public:
  PhysicsRuntime(Scene &aScene);
  ~PhysicsRuntime();
  void Update(float aDeltaTime);
  inline void SetMode(RuntimeMode aRuntimeMode) {
    myRuntimeMode = aRuntimeMode;
  }
  inline void Reset() {
    myTimeAccumulator = 0.0f;
    myStepClock = 0;
  }
private:

  void RunAuto(float aDeltaTime);
  void RunStepped();
  void SimulateOneStep();

private:
  PhysicsEngine* myEngine; 
  RuntimeMode myRuntimeMode = RuntimeMode_RUNNING;
  bool myCanRunNextStep = false;

  double myTimeAccumulator = 0.0f;
  uint8_t myStepsPerSecond = 1;
  uint32_t myStepClock = 0;
};
