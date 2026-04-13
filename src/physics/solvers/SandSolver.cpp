#include "SandSolver.h"

#include "../VoxelContext.h"

bool SandSolver::Solve(const VoxelContext& ctx){
    // Fall down if empty below
    if(myBasicSolver.FallDown(ctx))
        return true;

    if(SinkThroughWater(ctx))
        return true;

    return myBasicSolver.FallDiagonally(ctx);
}

bool SandSolver::SinkThroughWater(const VoxelContext& ctx){
    if (ctx.gridPos.y <= 0)
        return false;

    // only move down if water under
    size_t indexUnder = ctx.index - CHUNK_SIZE;
    if (ctx.src[indexUnder] != VoxelType_WATER)
        return false;

    // swap them in the dst
    ctx.dst[ctx.index]        = VoxelType_WATER;
    ctx.dst[indexUnder]       = VoxelType_SAND;
    ctx.actionMap[indexUnder] = (uint8_t) VoxelAction::SinkThruWater;

    return true;
}


