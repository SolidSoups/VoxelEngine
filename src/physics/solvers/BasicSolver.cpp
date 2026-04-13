#include "BasicSolver.h"
#include "physics/VoxelContext.h"

bool BasicSolver::FallDown(const VoxelContext &ctx)
{
    // get voxel position, skip bottom
    if (ctx.gridPos.y == 0)
        return false;

    // only move down on empty
    VoxelIndex indexUnder = ctx.index - CHUNK_SIZE;
    if (ctx.dst[indexUnder] != VoxelType_EMPTY)
        return false;

    // swap this voxel with the one under in the dst
    ctx.dst[ctx.index]        = ctx.dst[indexUnder];
    ctx.dst[indexUnder]       = ctx.voxel;
    ctx.actionMap[indexUnder] = (uint8_t) VoxelAction::FallDown;
    return true;
}

bool BasicSolver::FallDiagonally(const VoxelContext &ctx)
{
    // we are at the bottom, skip
    if (ctx.gridPos.y == 0)
        return false;

    size_t chosen;
    if (!FindDiagonalTarget(ctx, chosen))
        return false;

    // swap
    ctx.dst[ctx.index]    = ctx.dst[chosen];
    ctx.dst[chosen]       = ctx.voxel;
    ctx.actionMap[chosen] = (uint8_t) VoxelAction::FallDiagonally;

    return true;
}

bool BasicSolver::FindHorizontalTarget(const VoxelContext &ctx, size_t& aChosenIndex)
{
    VoxelIndex candidates[8];
    size_t     count = 0;

    // cache index under and the size of a z slice
    const VoxelIndex indexUnder = ctx.index - CHUNK_SIZE;
    const VoxelIndex zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    constexpr glm::ivec3 HorizontalTargets[8] = {
        {0,  0, 1},  // +z
        {-1, 0, 0}, // -x
        {0,  0, -1}, // -z
        {1,  0, 0},  // +x
        {-1, 0, 1}, // -x, +z
        {1,  0, 1},  // +x, +z
        {1,  0, -1}, // +x, -z
        {-1, 0, -1} // -x, -z
    };

    for (int i = 0; i < 8; i++)
    {
        glm::ivec3 gridPos     = ctx.gridPos + HorizontalTargets[i];
        size_t     neighborIdx = getVoxelIndex(gridPos);
        if (isOutOfBounds(gridPos) or ctx.dst[neighborIdx] != VoxelType_EMPTY)
        {
            continue;
        }

        candidates[count++] = neighborIdx;
    }

    // no candidates, return
    if (count == 0)
        return false;

    // swap a random diagonal candidate with this voxel
    aChosenIndex = candidates[rand() % count];
    return true;
}

bool BasicSolver::FindDiagonalTarget(const VoxelContext &ctx, size_t& aChosenIndex)
{
    VoxelIndex candidates[8];
    size_t     count = 0;

    // cache index under and the size of a z slice
    const VoxelIndex indexUnder = ctx.index - CHUNK_SIZE;
    const VoxelIndex zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    constexpr glm::ivec3 DiagonalTargets[8] = {
        {0, -1, 1},  // +z
        {-1, -1, 0}, // -x
        {0, -1, -1}, // -z
        {1, -1, 0},  // +x
        {-1, -1, 1}, // -x, +z
        {1, -1, 1},  // +x, +z
        {1, -1, -1}, // +x, -z
        {-1, -1, -1} // -x, -z
    };

    for (int i = 0; i < 8; i++)
    {
        glm::ivec3 gridPos     = ctx.gridPos + DiagonalTargets[i];
        size_t     neighborIdx = getVoxelIndex(gridPos);
        if (isOutOfBounds(gridPos) or ctx.dst[neighborIdx] != VoxelType_EMPTY)
        {
            continue;
        }

        candidates[count++] = neighborIdx;
    }

    // no candidates, return
    if (count == 0)
        return false;

    // swap a random diagonal candidate with this voxel
    aChosenIndex = candidates[rand() % count];
    return true;
}

bool BasicSolver::PickTargetFromDir(const VoxelContext &ctx, const glm::vec2 &aDir, size_t &outCandidate)
{
    const int zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    int signX = glm::sign(aDir.x);
    int signZ = glm::sign(aDir.y);

    bool canMoveX = signX != 0 and (ctx.gridPos.x + signX) >= 0 and (ctx.gridPos.x + signX) < CHUNK_SIZE;
    bool canMoveZ = signZ != 0 and (ctx.gridPos.z + signZ) >= 0 and (ctx.gridPos.z + signZ) < CHUNK_SIZE;

    // prefer true diagonal
    if (canMoveX and canMoveZ)
    {
        size_t diagonalIdx = ctx.index + signX + signZ * zSliceSize;
        if (ctx.dst[diagonalIdx] == VoxelType_EMPTY)
        {
            outCandidate = diagonalIdx;
            return true;
        }
    }

    // diagonal is blocked, fall back to strongest cardinal axis
    size_t candidates[2];
    size_t count = 0;

    if (canMoveX and ctx.dst[ctx.index + signX] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + signX;
    }
    if (canMoveZ and ctx.dst[ctx.index + signZ * CHUNK_SIZE] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + signZ * CHUNK_SIZE;
    }

    if (count == 0)
        return false;

    // Pick dominant axis
    size_t chosen;
    if (count == 1)
        chosen = candidates[0];
    else
        chosen = (fabs(aDir.x) >= fabs(aDir.y)) ? candidates[0] : candidates[1];

    outCandidate = (count == 1 or fabs(aDir.x) >= fabs(aDir.y)) ? candidates[0] : candidates[1];
    return true;
}
