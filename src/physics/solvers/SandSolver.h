#pragma once

#include "physics/solvers/BasicSolver.h"
struct VoxelContext;

class SandSolver
{
  public:
    bool Solve(const VoxelContext &ctx);

  private:
    bool SinkThroughWater(const VoxelContext& ctx);
  private:
    BasicSolver myBasicSolver;
};
