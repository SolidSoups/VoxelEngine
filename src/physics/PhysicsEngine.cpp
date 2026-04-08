#include "physics/PhysicsEngine.h"

#include <glm/glm.hpp>
#include "voxel/VoxelChunk.h"
#include "rendering/Scene.h"
#include "voxel/VoxelChunkViews.h"
#include "voxel/VoxelType.h"

#include "VoxelPhysics.h"

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene)
{
    xyMovedVoxels = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]();
    myXZHeightMap = new uint8_t[CHUNK_SIZE * CHUNK_SIZE]();
    myXZSlopeMap  = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
}
PhysicsEngine::~PhysicsEngine()
{
    delete[] xyMovedVoxels;
    delete[] myXZHeightMap;
    delete[] myXZSlopeMap;
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

    // Iterate for every z, from the bottom to top...
    for (VoxelIndex y = 0; y < chunkSize; y++)
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
                                 .xzSurfaceSlopeMap  = myXZSlopeMap};

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
                    }
                    break;
                }
            }
        }

    if (somethingMoved)
        dst.isDirty = true;

    myScene.CopyChunk(dst);
}

bool PhysicsEngine::SimulateSand(const VoxelContext &ctx)
{
    // Fall through empty
    if (FallDown(ctx))
        return true;

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
    // Stability check
    if(CountHorizontalNeighbours(ctx) >= 3)
        return false; // no movement
    if (SurfaceWaterSpread(ctx))
        return true;

    // return SpreadHorizontally(ctx);
    return false;
}

void PhysicsEngine::CreateHeightMap(const VoxelChunkViews &someViews)
{
    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            // find what height starts at
            VoxelBitset column = someViews.xzOccupancy[x + z * CHUNK_SIZE];
            size_t      start  = std::countr_zero(column);

            // if the start is 64, then there is nothing in this column and
            // right shifting will cause Undefined Behaviour
            if (start == CHUNK_SIZE)
            {
                myXZHeightMap[x + z * CHUNK_SIZE] = 0;
                continue;
            }

            // get the total height
            size_t size                       = std::countr_one(column << start);
            int    height                     = start + size;
            myXZHeightMap[x + z * CHUNK_SIZE] = height + 1;
        }
    }
}

void PhysicsEngine::CreateSlopeMap()
{
    // radius excluding center, ie would span a 3x3 grid
    constexpr int SAMPLE_RADIUS = 10;
    constexpr int TOTAL_SAMPLES = (SAMPLE_RADIUS * 2 + 1)*(SAMPLE_RADIUS * 2 + 1);

    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            uint8_t columnHeight = myXZHeightMap[x + z * CHUNK_SIZE];

            // sample neighbouring cells for their height, pick the lowest one
            int slopeX = 0, slopeZ = 0;
            for (int dz = -SAMPLE_RADIUS; dz <= SAMPLE_RADIUS; dz++)
            {
                for (int dx = -SAMPLE_RADIUS; dx <= SAMPLE_RADIUS; dx++)
                {
                    if (dx == 0 and dz == 0)
                        continue; // skip center cell

                    int nx = x + dx;
                    int nz = z + dz;
                    if (nx < 0 or nx >= CHUNK_SIZE or nz < 0 or nz >= CHUNK_SIZE)
                        continue; // skip invalid cells
                    
                    int heightDiff = columnHeight - myXZHeightMap[nx + nz * CHUNK_SIZE];
                    slopeX += dx * heightDiff;
                    slopeZ += dz * heightDiff;
                }
            }

            // store slope direction
            myXZSlopeMap[x + z * CHUNK_SIZE] = glm::vec2((float)slopeX / TOTAL_SAMPLES, (float)slopeZ / TOTAL_SAMPLES);
        }
    }
}
