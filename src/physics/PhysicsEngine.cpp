#include "physics/PhysicsEngine.h"

#include <immintrin.h>

#include <glm/glm.hpp>
#include <print>
#include "voxel/VoxelChunk.h"
#include "rendering/Scene.h"
#include "voxel/VoxelChunkViews.h"
#include "voxel/VoxelType.h"

#include "VoxelPhysics.h"

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene)
{
    xyMovedVoxels   = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]();
    myXZHeightMap   = new uint8_t[CHUNK_SIZE * CHUNK_SIZE]();
    myXZSlopeMap    = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myXZFDASlopeMap = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myActionMap     = new uint8_t[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE]();
}
PhysicsEngine::~PhysicsEngine()
{
    delete[] xyMovedVoxels;
    delete[] myXZHeightMap;
    delete[] myXZSlopeMap;
    delete[] myXZFDASlopeMap;
    delete[] myActionMap;
}

void PhysicsEngine::SimulateChunk()
{
    // We swap which direction on x-z plane we iterate
    // every frame
    myFrameCounter++;
    bool reverseZ       = myFrameCounter % 2;
    bool reverseX       = (myFrameCounter / 2) % 2;
    bool somethingMoved = false;

    // clear moved voxels
    memset(xyMovedVoxels, 0, CHUNK_SIZE * CHUNK_SIZE * sizeof(VoxelBitset));

    // create src and dst
    const VoxelChunk &src       = myScene.GetVoxelChunk();
    const VoxelIndex  chunkSize = src.GetWidth();
    VoxelChunk        dst{src}; // copy src into dst so unmoved voxels still exist

    CreateHeightMap(myScene.GetChunkViews());
    CreateSlopeMap();
    CreateSlopeMap_fda_d8(myScene.GetChunkViews().xzOccupancy);

    // clear the action map
    memset(myActionMap, 0, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

    // Iterate for every z, from the bottom to top...
    for (VoxelIndex y = 0; y < chunkSize; y++)
    {
        for (VoxelIndex iz = 0; iz < chunkSize; iz++)
        {
            VoxelIndex z = reverseZ ? (chunkSize - 1 - iz) : iz;
            for (VoxelIndex ix = 0; ix < chunkSize; ix++)
            {
                VoxelIndex x = reverseX ? (chunkSize - 1 - ix) : ix;
                // iterate every voxel in this vertical chunk slice...
                size_t index = x + y * chunkSize + z * chunkSize * chunkSize;
                Voxel  voxel = src[index];

                // skip all empty voxels
                if (voxel == VoxelType_EMPTY)
                    continue;

                // create context
                VoxelContext ctx{.dst                = dst,
                                 .src                = src,
                                 .voxel              = voxel,
                                 .index              = x + y * chunkSize + z * chunkSize * chunkSize,
                                 .gridPos            = getVoxelGridPosition(index),
                                 .xzSurfaceHeightMap = myXZHeightMap,
                                 .xzSurfaceSlopeMap  = myXZSlopeMap,
                                 .xzFDASlopeMap      = myXZFDASlopeMap,
                                 .actionMap          = myActionMap};

                // Simulate voxels
                switch (ctx.voxel)
                {
                case (VoxelType_SAND):
                    if (SimulateSand(ctx))
                    {
                        somethingMoved = true;
                    }
                    break;
                case (VoxelType_WATER):
                    if (SimulateWater(ctx))
                    {
                        somethingMoved = true;
                        xyMovedVoxels[x + z * CHUNK_SIZE] |= (VoxelBitset(1) << y);
                    }
                    break;
                }
            }
        }
    }

    // rebuild voxel chunk
    if (somethingMoved)
        dst.isDirty = true;
    myScene.CopyChunk(dst);
    myScene.RebuildViews();
    CreateHeightMap(myScene.GetChunkViews());
    CreateSlopeMap_fda_d8(myScene.GetChunkViews().xzOccupancy);
}

bool PhysicsEngine::SimulateSand(const VoxelContext &ctx)
{
    // Fall through empty
    if (FallDown(ctx))
    {
        return true;
    }

    // Sink through water
    if (SinkThroughWater(ctx))
        return true;

    return FallDiagonally(ctx);
}

bool PhysicsEngine::SimulateWater(const VoxelContext &ctx)
{
    if (FallDown(ctx))
        return true;

    // check pressure (the amount of water blocks above), prioritize
    // spreading horizontally if covered
    bool underPressure = ctx.gridPos.y < CHUNK_SIZE - 1 and ctx.dst[ctx.index + CHUNK_SIZE] == VoxelType_WATER;
    if (underPressure)
    {
        if (SpreadHorizontally(ctx))
            return true;
        return FallDiagonally(ctx);
    }

    // surface water, fall diagonally first, then spread
    if (FallDiagonally(ctx))
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

PhysicsDebugData PhysicsEngine::GetDebugData() const
{
    return {myXZHeightMap, myXZSlopeMap, xyMovedVoxels, myActionMap, myXZFDASlopeMap};
}

void PhysicsEngine::CreateHeightMap(const VoxelChunkViews &someViews)
{
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i += 2)
    {
        // calc height map
        myXZHeightMap[i]     = CHUNK_SIZE - _lzcnt_u64(someViews.xzOccupancy[i]);
        myXZHeightMap[i + 1] = CHUNK_SIZE - _lzcnt_u64(someViews.xzOccupancy[i + 1]);
    }
}

void PhysicsEngine::CreateSlopeMap_fda_d8(const VoxelBitset *xzOccupancy)
{
    static int frameCounter = 0;
    frameCounter++;
    constexpr int MAX_STEPS = 10;

    // perform FDA for every cell
    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {

            uint8_t cellHeight = CHUNK_SIZE - (uint8_t) _lzcnt_u64(xzOccupancy[x + z * CHUNK_SIZE]);
            if (cellHeight <= 1)
            {
                myXZFDASlopeMap[x + z * CHUNK_SIZE] = glm::vec2(0);
                continue;
            }

            // find the neighbor which FDA D8 path leads to the lowest point
            uint8_t    bestFinalHeight = cellHeight;
            glm::ivec2 bestFirstStep{x, z};
            for (int deltaZ = -1; deltaZ <= 1; deltaZ++)
            {
                for (int deltaX = -1; deltaX <= 1; deltaX++)
                {
                    // skip center
                    if (deltaX == 0 and deltaZ == 0)
                        continue;

                    int neighborX = x + deltaX;
                    int neighborZ = z + deltaZ;
                    if (neighborX < 0 or neighborX >= CHUNK_SIZE or neighborZ < 0 or neighborZ >= CHUNK_SIZE)
                    {
                        continue; // neighbors don't exist out of bounds
                    }

                    // we must be able to move there,
                    // if heights are equal it is filled
                    uint8_t neighborHeight = CHUNK_SIZE - (uint8_t) _lzcnt_u64(xzOccupancy[neighborX + neighborZ * CHUNK_SIZE]);
                    if (bestFinalHeight > neighborHeight)
                    {
                        bestFinalHeight = neighborHeight;
                        bestFirstStep = {neighborX, neighborZ};
                    }

                    // // We need to be able to reach the neighbor, even if it is a corner and we need to go through
                    // // a couple of steps
                    // bool neighborIsCorner = deltaX != 0 and deltaZ != 0;
                    // if (neighborIsCorner)
                    // {
                    //     int  centerXHeight       = CHUNK_SIZE - _lzcnt_u64(xzOccupancy[x + neighborZ * CHUNK_SIZE]);
                    //     int  centerZHeight       = CHUNK_SIZE - _lzcnt_u64(xzOccupancy[neighborX + z * CHUNK_SIZE]);
                    //     bool neighborIsReachable = (centerXHeight < cellHeight and
                    //                                 neighborHeight <= centerXHeight) or // full height path from centerX
                    //                                (centerZHeight < cellHeight and
                    //                                 neighborHeight <= centerZHeight); // full height path from centerZ
                    //     if (not neighborIsReachable)
                    //     {
                    //         continue;
                    //     }
                    // }

                    // simulate remaining steps from this first step
                    int lowestHeight = FDASimulateSteps(neighborX, neighborZ, neighborHeight, xzOccupancy, MAX_STEPS);
                    if (lowestHeight < bestFinalHeight)
                    {
                        bestFinalHeight = lowestHeight;
                        bestFirstStep   = {neighborX, neighborZ};
                    }
                }
            }

            // slope is 0 if we didnt move at all
            if ((bestFirstStep.x == x and bestFirstStep.y == z) or bestFinalHeight >= cellHeight-1)
                myXZFDASlopeMap[x + z * CHUNK_SIZE] = glm::vec2(0.f);
            // set slope if we did
            else
                myXZFDASlopeMap[x + z * CHUNK_SIZE] = (glm::vec2) bestFirstStep - glm::vec2(x, z);
        }
    }
}

int PhysicsEngine::FDASimulateSteps(int x, int z, int height, const VoxelBitset *xzOccupancy, int maxSteps)
{
    uint8_t y = height - 1;

    uint8_t    walkHeight = height;
    glm::ivec2 walkPosition{x, z};
    for (int step = 1; step < maxSteps; step++)
    {
        bool       hasMoved     = false;
        uint8_t    centerHeight = walkHeight;
        glm::ivec2 centerPos    = walkPosition;
        for (int deltaZ = -1; deltaZ <= 1; deltaZ++)
        {
            for (int deltaX = -1; deltaX <= 1; deltaX++)
            {

                // skip center
                if (deltaX == 0 and deltaZ == 0)
                    continue;

                int neighborX = deltaX + centerPos.x;
                int neighborZ = deltaZ + centerPos.y;

                // skip invalid cells
                if (neighborX < 0 or neighborX >= CHUNK_SIZE or neighborZ < 0 or neighborZ >= CHUNK_SIZE)
                    continue;

                int neighborHeight = CHUNK_SIZE - _lzcnt_u64(xzOccupancy[neighborX + neighborZ * CHUNK_SIZE]);

                // is neighbor reachable?
                if (neighborHeight <= centerHeight)
                {
                    walkHeight   = neighborHeight;
                    walkPosition = {neighborX, neighborZ};
                    hasMoved     = true;
                }
            }
        }
        if (!hasMoved)
            break;
    }
    return walkHeight;
}

void PhysicsEngine::CreateSlopeMap()
{
    constexpr int R   = 8;
    constexpr int PAD = CHUNK_SIZE + R * 2;

    // padded height map. used to avoid branching on invalid voxel positions
    // the padded edge around the chunk border will maintain the borders value,
    // causing no height difference and falling off slope
    uint8_t padded[PAD * PAD] = {};
    for (int z = 0; z < PAD; z++)
    {
        for (int x = 0; x < PAD; x++)
        {
            int cx              = std::clamp(x - R, 0, CHUNK_SIZE - 1);
            int cz              = std::clamp(z - R, 0, CHUNK_SIZE - 1);
            padded[z * PAD + x] = myXZHeightMap[cx + cz * CHUNK_SIZE];
        }
    }

    // for a 9x9, minus the center, we have 80 samples.
    //
    int16_t accumX[CHUNK_SIZE * CHUNK_SIZE] = {};
    int16_t accumZ[CHUNK_SIZE * CHUNK_SIZE] = {};
    int16_t count[CHUNK_SIZE * CHUNK_SIZE]  = {};

    // For each eighbor offset...
    for (int dz = -R; dz <= R; dz++)
    {
        for (int dx = -R; dx <= R; dx++)
        {
            // ... accumulate their contribution
            if (dx == 0 and dz == 0)
                continue;

            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                // get the center and neigbor heights from the padded buffer
                const uint8_t *center    = &padded[(z + R) * PAD + R];
                const uint8_t *neighbor  = &padded[(z + R + dz) * PAD + R + dx];
                int            baseIndex = z * CHUNK_SIZE;

                // accumulate contributon with auto-vectorization optmized flow
                // no branches, linear memory access
                for (int x = 0; x < CHUNK_SIZE; x++)
                {
                    uint8_t diff = center[x] > neighbor[x] ? center[x] - neighbor[x] : 0;
                    accumX[baseIndex + x] += dx * diff;
                    accumZ[baseIndex + x] += dz * diff;
                    count[baseIndex + x] += diff > 0;
                }
            }
        }
    }

    // calculate slopes after
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
    {
        myXZSlopeMap[i] =
            count[i] > 0 ? glm::vec2((float) accumX[i] / count[i], (float) accumZ[i] / count[i]) : glm::vec2(0.0f);
    }
}
