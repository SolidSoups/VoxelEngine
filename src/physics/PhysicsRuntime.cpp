#include "physics/PhysicsRuntime.h"

#include <print>
#include "physics/PhysicsEngine.h"
#include "rendering/Scene.h"

PhysicsRuntime::PhysicsRuntime(Scene &aScene) {
  myEngine = new PhysicsEngine(aScene);
}
PhysicsRuntime::~PhysicsRuntime() {
  if (myEngine)
    delete myEngine;
}

void PhysicsRuntime::Update(float aDeltaTime) {
  if (myRuntimeMode == RuntimeMode_RUNNING) {
    RunAuto(aDeltaTime);
  } else if (myRuntimeMode == RuntimeMode_PAUSED) {
    RunStepped();
  } else if (myRuntimeMode == RuntimeMode_STOPPED)
    return;
}
void PhysicsRuntime::RunStepped() {
  if (myCanRunNextStep) {
    SimulateOneStep();
    myCanRunNextStep = false;
  }
}

void PhysicsRuntime::RunAuto(float aDeltaTime) {
  // we don't want division by zero
  if (stepsPerSecond == 0) {
    std::println(stderr, "Steps per second is zero, cannot step!");
    return;
  }

  // get the amount of seconds per step per second
  const double tickRate = 1.0 / stepsPerSecond;

  // run accumulation loop
  myTimeAccumulator += aDeltaTime;
  while (myTimeAccumulator >= tickRate) {
    SimulateOneStep();
    myTimeAccumulator -= tickRate;
  }
}

void PhysicsRuntime::SimulateOneStep() {
  myEngine->SimulateChunk();
  myStepClock++;
}
