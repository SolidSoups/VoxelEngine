#pragma once

struct VoxelContext;

class BasicSolver
{
  public:
    bool FallDown(const VoxelContext &ctx);
    bool FallDiagonally(const VoxelContext &ctx);
    bool FindHorizontalTarget(const VoxelContext &ctx, size_t &aChosenIndex);
    bool FindDiagonalTarget(const VoxelContext &ctx, size_t &aChosenIndex);
    bool PickTargetFromDir(const VoxelContext &ctx, const glm::vec2 &aDir, size_t &outCandidate);
};
