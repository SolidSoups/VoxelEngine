#include "physics/PhysicsEngine.h"

#include <immintrin.h>

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
                        xyMovedVoxels[x + z * CHUNK_SIZE] |= (VoxelBitset(1) << y);
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
    if (SurfaceWaterSpread(ctx))
        return true;

    return SpreadHorizontally(ctx);
}

PhysicsDebugData PhysicsEngine::GetDebugData() const
{
    return {myXZHeightMap, myXZSlopeMap, xyMovedVoxels};
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

void PhysicsEngine::CreateSlopeMap()
{
    constexpr int R   = 8;
    constexpr int PAD = CHUNK_SIZE + R * 2;

    // padded height map. used to avoid branching on invalid voxel positions
    // the padded edge around the chunk border will maintain the borders value,
    // causing no height difference and falling off slope
    uint8_t padded[PAD * PAD] = {};
    for(int z=0; z<PAD; z++){
        for(int x=0; x<PAD; x++){
            int cx=std::clamp(x - R, 0, CHUNK_SIZE-1);
            int cz=std::clamp(z - R, 0, CHUNK_SIZE-1);
            padded[z * PAD + x] = myXZHeightMap[cx + cz*CHUNK_SIZE];
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
                    accumX[baseIndex + x] += dx*diff;
                    accumZ[baseIndex + x] += dz*diff;
                    count[baseIndex+x] += diff > 0 ;
                }
            }
        }
    }

    // calculate slopes after
    for(int i=0; i<CHUNK_SIZE*CHUNK_SIZE; i++){
        myXZSlopeMap[i] = count[i] > 0 
            ? glm::vec2((float)accumX[i] / count[i], (float)accumZ[i] / count[i])
            : glm::vec2(0.0f);
    }
}
