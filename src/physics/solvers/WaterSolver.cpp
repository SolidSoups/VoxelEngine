#include "WaterSolver.h"

#include "../VoxelContext.h"

bool WaterSolver::Solve(const VoxelContext& ctx){
    if(myBasicSolver.FallDown(ctx))
        return true;

    // check pressure (the amount of water blocks above), prioritize
    // spreading horizontally if covered
    bool underPressure = ctx.gridPos.y < CHUNK_SIZE - 1 and ctx.dst[ctx.index + CHUNK_SIZE] == VoxelType_WATER;
    if (underPressure)
    {
        if (SpreadHorizontally(ctx))
            return true;
        return myBasicSolver.FallDiagonally(ctx);
    }

    // surface water, fall diagonally first, then spread
    if (myBasicSolver.FallDiagonally(ctx))
        return true;

    bool isBlockUnder = ctx.gridPos.y > 0 and ctx.dst[ctx.index - CHUNK_SIZE] != VoxelType_EMPTY;
    bool isBlockAbove = ctx.gridPos.y < CHUNK_SIZE - 1 and ctx.dst[ctx.index + CHUNK_SIZE] != VoxelType_EMPTY;
    if (isBlockUnder and not isBlockAbove)
    {
        if (SurfaceWaterSpread(ctx))
            return true;
    }

    return SpreadHorizontally(ctx);
}

bool WaterSolver::SurfaceWaterSpread(const VoxelContext& ctx){
    // if nothing below, we should've fallen instead
    if (ctx.gridPos.y > 0 and ctx.dst[ctx.index - CHUNK_SIZE] == VoxelType_EMPTY)
        return false;

    // Spread torwards deeper slopes. If no slopes exist, clump together torwards
    // the center of mass.
    glm::vec2 dir    = ctx.xzBFSSlopeMap[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
    if (dir.x == 0 and dir.y == 0)
        return false;

    // Pick dominant axis
    size_t chosenIdx;
    if (!myBasicSolver.PickTargetFromDir(ctx, dir, chosenIdx))
    {
        return false;
    }

    // // ensure that the chosen position actually allows us to fall down
    int     myHeight        = ctx.xzSurfaceHeightMap[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
    int     nx              = chosenIdx % CHUNK_SIZE;
    int     nz              = chosenIdx / (CHUNK_SIZE * CHUNK_SIZE);
    uint8_t neighbourHeight = ctx.xzSurfaceHeightMap[nx + nz * CHUNK_SIZE];
    int     diff            = myHeight - neighbourHeight;
    if (diff <= 0)
        return false;

    // swap with chosen in dst buffer
    ctx.dst[ctx.index]       = ctx.dst[chosenIdx];
    ctx.dst[chosenIdx]       = ctx.voxel;
    ctx.actionMap[chosenIdx] = (uint8_t) VoxelAction::SurfaceSpread;

    return true;
}

bool WaterSolver::SpreadHorizontally(const VoxelContext& ctx){
    // wave-driven spread for surface voxels 
    uint8_t surfaceY = ctx.xzSurfaceHeightMap[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
    bool isSurface = (uint8_t)ctx.gridPos.y == surfaceY - 1;
    if(isSurface){
        glm::vec2 vel = ctx.xzWaveVelocity[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
        if(glm::dot(vel, vel) > 0.01f){
            // if(rand() % 3 == 0)
            //     return false;
            size_t chosenIdx;
            if(myBasicSolver.PickTargetFromDir(ctx, vel, chosenIdx)){
                ctx.dst[ctx.index] = ctx.dst[chosenIdx];
                ctx.dst[chosenIdx] = ctx.voxel;
                ctx.actionMap[chosenIdx] = (uint8_t)VoxelAction::SpreadH;
                return true;
            }
        }
    }
    return false;


    // Add more spread if above is water ???
    bool aboveIsWater = ctx.gridPos.y < CHUNK_SIZE - 1 && ctx.dst[ctx.index + CHUNK_SIZE] == VoxelType_WATER;
    bool weAreWater   = ctx.voxel == VoxelType_WATER;
    int  spreadChance = aboveIsWater and weAreWater ? 1 : 3;
    if (rand() % spreadChance != 0)
        return false;

    // nothing below, keep falling. but only if we can still fall there
    if (ctx.gridPos.y > 0 and ctx.dst[ctx.index - CHUNK_SIZE] == VoxelType_EMPTY)
        return false;

    size_t chosen;
    if (!myBasicSolver.FindHorizontalTarget(ctx, chosen))
    {
        return false;
    }
    ctx.dst[ctx.index]    = ctx.dst[chosen];
    ctx.dst[chosen]       = ctx.voxel;
    ctx.actionMap[chosen] = (uint8_t) VoxelAction::SpreadH;

    return true;
}
