#pragma once

#include "physics/solvers/BasicSolver.h"
struct VoxelContext;

class WaterSolver{
public:
    bool Solve(const VoxelContext& ctx);
private:
    bool SurfaceWaterSpread(const VoxelContext& ctx);
    bool SpreadHorizontally(const VoxelContext& ctx);
private:
    BasicSolver myBasicSolver;
};
