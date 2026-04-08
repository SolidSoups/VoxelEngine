#include "VoxelPhysics.h"
#include "../voxel/VoxelChunk.h"

bool SinkThroughWater(const VoxelContext &ctx)
{
    if (ctx.gridPos.y <= 0)
        return false;

    // only move down if water under
    size_t indexUnder = ctx.index - CHUNK_SIZE;
    if (ctx.src[indexUnder] != VoxelType_WATER)
        return false;

    // swap them in the dst
    ctx.dst[ctx.index]  = VoxelType_WATER;
    ctx.dst[indexUnder] = VoxelType_SAND;

    return true;
}

bool FallDown(const VoxelContext &ctx)
{
    // get voxel position, skip bottom
    if (ctx.gridPos.y == 0)
        return false;

    // only move down on empty
    VoxelIndex indexUnder = ctx.index - CHUNK_SIZE;
    if (ctx.dst[indexUnder] != VoxelType_EMPTY)
        return false;

    // swap this voxel with the one under in the dst
    ctx.dst[ctx.index]  = ctx.dst[indexUnder];
    ctx.dst[indexUnder] = ctx.voxel;
    return true;
}

bool FallDiagonally(const VoxelContext &ctx)
{
    // we are at the bottom, skip
    if (ctx.gridPos.y == 0)
        return false;

    VoxelIndex chosen;
    if (!FindDiagonalTarget(ctx, chosen))
        return false;

    // swap
    ctx.dst[ctx.index] = ctx.dst[chosen];
    ctx.dst[chosen]    = ctx.voxel;

    return true;
}

bool SpreadHorizontally(const VoxelContext &ctx)
{
    // Add more spread if above is water ???
    bool aboveIsWater = ctx.gridPos.y < CHUNK_SIZE - 1 && ctx.dst[ctx.index + CHUNK_SIZE] == VoxelType_WATER;
    bool weAreWater   = ctx.voxel == VoxelType_WATER;
    int  spreadChance = aboveIsWater and weAreWater ? 1 : 3;
    if (rand() % spreadChance != 0)
        return false;

    // nothing below, keep falling. but only if we can still fall there
    if (ctx.gridPos.y > 0 and ctx.dst[ctx.index - CHUNK_SIZE] == VoxelType_EMPTY)
        return false;

    VoxelIndex chosen;
    if (!FindHorizontalTarget(ctx, chosen))
    {
        return false;
    }
    ctx.dst[ctx.index] = ctx.dst[chosen];
    ctx.dst[chosen]    = ctx.voxel;
    return true;
}

//
// 1. Count water neighbouring water voxels
//
bool SurfaceWaterSpread(const VoxelContext &ctx)
{
    // if nothing below, we should've fallen instead
    if (ctx.gridPos.y > 0 and ctx.dst[ctx.index - CHUNK_SIZE] == VoxelType_EMPTY)
        return false;

    // Spread torwards deeper slopes. If no slopes exist, clump together torwards
    // the center of mass.
    glm::vec2 dir = ctx.xzSurfaceSlopeMap[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
    if (dir.x == 0 and dir.y == 0)
        return false;
    //     dir = CalcCenterOfMass(ctx);

    // Pick dominant axis
    size_t chosenIdx;
    if (!PickTargetFromDir(ctx, dir, chosenIdx))
    {
        return false;
    }

    // ensure that the chosen position actually allows us to fall down
    int myHeight = ctx.xzSurfaceHeightMap[ctx.gridPos.x + ctx.gridPos.z * CHUNK_SIZE];
    int nx = chosenIdx % CHUNK_SIZE;
    int nz = chosenIdx / (CHUNK_SIZE * CHUNK_SIZE);
    uint8_t neighbourHeight = ctx.xzSurfaceHeightMap[nx + nz * CHUNK_SIZE];
    int diff = myHeight - neighbourHeight;
    if(diff <= 0)
        return false;


    // swap with chosen in dst buffer
    ctx.dst[ctx.index] = ctx.dst[chosenIdx];
    ctx.dst[chosenIdx]    = ctx.voxel;
    return true;
}

bool PickTargetFromDir(const VoxelContext &ctx, const glm::vec2 &aDir, size_t &outCandidate)
{
    // move torwards the center of mass (where the water is)
    // mimics surface tension
    size_t candidates[2];
    size_t count      = 0;
    int    zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    // pick directions based on the center of mass
    // positive comX => move closer to the right
    if (aDir.x > 0.f and ctx.gridPos.x < CHUNK_SIZE - 1 and ctx.dst[ctx.index + 1] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + 1;
    }
    else if (aDir.x < 0.f and ctx.gridPos.x > 0 and ctx.dst[ctx.index - 1] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index - 1;
    }

    if (aDir.y > 0.f and ctx.gridPos.z < CHUNK_SIZE - 1 and ctx.dst[ctx.index + zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + zSliceSize;
    }
    else if (aDir.y < 0.f and ctx.gridPos.z > 0 and ctx.dst[ctx.index - zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index - zSliceSize;
    }

    if (count == 0)
        return false;

    // Pick dominant axis
    size_t chosen;
    if (count == 1)
        chosen = candidates[0];
    else
        chosen = (fabs(aDir.x) >= fabs(aDir.y)) ? candidates[0] : candidates[1];

    outCandidate = chosen;
    return true;
}

bool FindDiagonalTarget(const VoxelContext &ctx, VoxelIndex &outIndex)
{
    VoxelIndex candidates[4];
    size_t     count = 0;

    // cache index under and the size of a z slice
    const VoxelIndex indexUnder = ctx.index - CHUNK_SIZE;
    const VoxelIndex zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    // check diagonal left
    if (ctx.gridPos.x > 0 && ctx.dst[indexUnder - 1] == VoxelType_EMPTY)
    {
        candidates[count++] = indexUnder - 1;
    }
    // check diagonal right
    if (ctx.gridPos.x < CHUNK_SIZE - 1 && ctx.dst[indexUnder + 1] == VoxelType_EMPTY)
    {
        candidates[count++] = indexUnder + 1;
    }
    // check diagonal front
    if (ctx.gridPos.z < CHUNK_SIZE - 1 && ctx.dst[indexUnder + zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = indexUnder + zSliceSize;
    }
    // check diagonal back
    if (ctx.gridPos.z > 0 && ctx.dst[indexUnder - zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = indexUnder - zSliceSize;
    }

    // no candidates, return
    if (count == 0)
        return false;

    // swap a random diagonal candidate with this voxel
    outIndex = candidates[rand() % count];
    return true;
}

bool FindHorizontalTarget(const VoxelContext &ctx, VoxelIndex &outIndex)
{
    VoxelIndex candidates[4];
    size_t     count = 0;

    const VoxelIndex zSliceSize = CHUNK_SIZE * CHUNK_SIZE;

    // Horizontally left
    if (ctx.gridPos.x > 0 and ctx.dst[ctx.index - 1] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index - 1;
    }
    // Horizontally right
    if (ctx.gridPos.x < CHUNK_SIZE - 1 and ctx.dst[ctx.index + 1] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + 1;
    }
    // Horizontally forward
    if (ctx.gridPos.z < CHUNK_SIZE - 1 and ctx.dst[ctx.index + zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index + zSliceSize;
    }
    // Horizontally back
    if (ctx.gridPos.z > 0 and ctx.dst[ctx.index - zSliceSize] == VoxelType_EMPTY)
    {
        candidates[count++] = ctx.index - zSliceSize;
    }

    // no candidates, return
    if (count == 0)
        return false;

    // swap a random horizontal candidate with this voxel
    outIndex = candidates[rand() % count];
    return true;
}

int CountHorizontalNeighbours(const VoxelContext &ctx)
{
    const int zSliceSize = CHUNK_SIZE * CHUNK_SIZE;
    int       count      = 0;

    if (ctx.gridPos.x - 1 >= 0 and ctx.dst[ctx.index - 1] != VoxelType_EMPTY)
        count++;
    if (ctx.gridPos.z - 1 >= 0 and ctx.dst[ctx.index - zSliceSize] != VoxelType_EMPTY)
        count++;
    if (ctx.gridPos.x + 1 < CHUNK_SIZE and ctx.dst[ctx.index + 1] != VoxelType_EMPTY)
        count++;
    if (ctx.gridPos.z + 1 < CHUNK_SIZE and ctx.dst[ctx.index + zSliceSize] != VoxelType_EMPTY)
        count++;

    return count;
}

glm::vec2 CalcCenterOfMass(const VoxelContext &ctx, int aRadius)
{
    const size_t zSliceSize = CHUNK_SIZE * CHUNK_SIZE;
    int          comX = 0, comZ = 0;
    for (int dz = -aRadius; dz <= aRadius; dz++)
    {
        for (int dx = -aRadius; dx <= aRadius; dx++)
        {
            if (dx == 0 && dz == 0)
                continue;
            int nx = ctx.gridPos.x + dx;
            int nz = ctx.gridPos.z + dz;
            if (nx < 0 or nx >= CHUNK_SIZE or nz < 0 or nz >= CHUNK_SIZE)
                continue;
            size_t ni = ctx.index + dx + dz * zSliceSize;
            if (ctx.dst[ni] == VoxelType_WATER)
            {
                comX += dx;
                comZ += dz;
            }
        }
    }
    float diam = aRadius * 2 + 1;
    return {(float) comX / diam, (float) comZ / diam};
}

// Calculates the direction of the deepest slope
glm::vec2 CalcSlopeDirection(const VoxelContext &ctx, int aRadius)
{
    // iterate on the grid, average positions of depth.
    // Deeper positions are amplified by multiplying with the depth,
    // ex. pos=(2,3), depth=2, result=(2*2, 2*3)
    const size_t zSliceSize = CHUNK_SIZE * CHUNK_SIZE;
    int          slopeX = 0, slopeZ = 0;
    for (int dz = -aRadius; dz <= aRadius; dz++)
    {
        for (int dx = -aRadius; dx <= aRadius; dx++)
        {
            if (dx == 0 and dz == 0)
                continue; // skip center

            // get actual position
            int nx = ctx.gridPos.x + dx;
            int nz = ctx.gridPos.z + dz;

            if (nx < 0 or nx >= CHUNK_SIZE or nz < 0 or nz >= CHUNK_SIZE)
                continue; // skip invalid cells

            // calc depth
            size_t idx   = ctx.index + dx + dz * zSliceSize;
            int    depth = ctx.xzSurfaceHeightMap[nx + nz * CHUNK_SIZE];

            // add to slope position
            slopeX += dx * depth / (abs(dx) + abs(dz));
            slopeZ += dz * depth / (abs(dx) + abs(dz));
        }
    }

    // average them
    int       diam = aRadius * 2 + 1;
    glm::vec2 slopeDir{(float) slopeX / diam, (float) slopeZ / diam};
    return slopeDir;
}
