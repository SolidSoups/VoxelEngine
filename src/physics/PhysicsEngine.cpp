#include "physics/PhysicsEngine.h"

#include <algorithm>
#include <immintrin.h>

#include <glm/glm.hpp>
#include "voxel/VoxelChunk.h"
#include "rendering/Scene.h"
#include "voxel/VoxelChunkViews.h"
#include "voxel/VoxelType.h"
#include "VoxelContext.h"
#include "editors/TextureViewer.h"

PhysicsEngine::PhysicsEngine(Scene &aScene) : myScene(aScene)
{
    myXZWaveVelocity = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myXZHeightMap    = new uint8_t[CHUNK_SIZE * CHUNK_SIZE]();
    myXZSlopeMap     = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myXZBFSSlopeMap  = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myActionMap      = new uint8_t[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE]();
    myGradStep = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myWaveStep = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    myCohesionStep = new glm::vec2[CHUNK_SIZE * CHUNK_SIZE]();
    
    TextureViewerData::Get().AddEntry<TextureEntry_uint8>(myActionMap, "Action Map", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myXZSlopeMap, "Slope Map", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myXZBFSSlopeMap, "BFS Slope Map", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myGradStep, "Wave Velocity Step 1 (Grad)", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_uint8>(myXZHeightMap, "Height Map", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myWaveStep, "Wave Velocity Step 2 (Wave)", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myCohesionStep, "Wave Velocity Step 3 (Cohesion)", CHUNK_SIZE, CHUNK_SIZE);
    TextureViewerData::Get().AddEntry<TextureEntry_vec2>(myXZWaveVelocity, "Wave Velocity Step 4 (Sum)", CHUNK_SIZE, CHUNK_SIZE);
}
PhysicsEngine::~PhysicsEngine()
{
    delete[] myXZWaveVelocity;
    delete[] myXZHeightMap;
    delete[] myXZSlopeMap;
    delete[] myActionMap;
    delete[] myXZBFSSlopeMap;
    delete[] myGradStep;
    delete[] myWaveStep;
    delete[] myCohesionStep;
}

void PhysicsEngine::SimulateChunk()
{
    // We swap which direction on x-z plane we iterate
    // every frame
    myFrameCounter++;
    bool reverseZ       = myFrameCounter % 2;
    bool reverseX       = (myFrameCounter / 2) % 2;
    bool somethingMoved = false;

    // create src and dst
    const VoxelChunk &src       = myScene.GetVoxelChunk();
    const VoxelIndex  chunkSize = src.GetWidth();
    VoxelChunk        dst{src}; // copy src into dst so unmoved voxels still exist

    CreateHeightMap(myScene.GetChunkViews());
    UpdateWaveVelocities();
    CreateSlopeMap();
    CreateBFSSlopeMap();

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
                                 .xzBFSSlopeMap      = myXZBFSSlopeMap,
                                 .xzWaveVelocity     = myXZWaveVelocity,
                                 .actionMap          = myActionMap};

                // Simulate voxels
                switch (ctx.voxel)
                {
                case (VoxelType_SAND):
                    if (mySandSolver.Solve(ctx))
                    {
                        somethingMoved = true;
                    }
                    break;
                case (VoxelType_WATER):
                    if (myWaterSolver.Solve(ctx))
                    {
                        somethingMoved = true;
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
    CreateBFSSlopeMap();
}

void PhysicsEngine::UpdateWaveVelocities()
{
    float     t   = myFrameCounter * 0.01f;
    glm::vec2 dir = waterSettings.waveDirection;

    // Diffusion pass: smooth velocity field across neighbors
    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            int i = x + z * CHUNK_SIZE;
            if (myXZHeightMap[i] == 0)
                continue;

            glm::vec2 neighborAvg{};
            int       count = 0;
            if (x > 0)            { neighborAvg += myXZWaveVelocity[(x - 1) + z * CHUNK_SIZE]; count++; }
            if (x < CHUNK_SIZE-1) { neighborAvg += myXZWaveVelocity[(x + 1) + z * CHUNK_SIZE]; count++; }
            if (z > 0)            { neighborAvg += myXZWaveVelocity[x + (z - 1) * CHUNK_SIZE]; count++; }
            if (z < CHUNK_SIZE-1) { neighborAvg += myXZWaveVelocity[x + (z + 1) * CHUNK_SIZE]; count++; }
            if (count > 0)
                myXZWaveVelocity[i] += waterSettings.diffusion * (neighborAvg / (float)count - myXZWaveVelocity[i]);
        }
    }

    // Force accumulation pass
    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            int i = x + z * CHUNK_SIZE;

            if (myXZHeightMap[i] == 0)
            {
                myXZWaveVelocity[i] *= waterSettings.waveDamping;
                continue;
            }

            // Terrain gradient: very gentle slope response
            float hL = x > 0            ? myXZHeightMap[(x - 1) + z * CHUNK_SIZE] : myXZHeightMap[i];
            float hR = x < CHUNK_SIZE-1 ? myXZHeightMap[(x + 1) + z * CHUNK_SIZE] : myXZHeightMap[i];
            float hB = z > 0            ? myXZHeightMap[x + (z - 1) * CHUNK_SIZE] : myXZHeightMap[i];
            float hF = z < CHUNK_SIZE-1 ? myXZHeightMap[x + (z + 1) * CHUNK_SIZE] : myXZHeightMap[i];
            glm::vec2 grad = {(hR - hL) * 0.5f, (hF - hB) * 0.5f};
            myGradStep[i] = -waterSettings.waveSpeed * grad;

            // Ambient wave: smooth sinusoidal push with organic warp across the wavefront
            glm::vec2 perp    = glm::vec2(-dir.y, dir.x);
            float parallelPos = (x * dir.x + z * dir.y) * waterSettings.waveFrequency;
            float perpPos     = (x * perp.x + z * perp.y);
            float posPhase    = parallelPos + 0.6f * glm::sin(perpPos * 0.3f + glm::sin(t * 0.1f));
            float surgeDir    = glm::sin(t * waterSettings.surgeSpeed);
            float wave        = glm::sin(t * waterSettings.realWaveSpeed * surgeDir + posPhase);
            myWaveStep[i]     = dir * waterSettings.ambientStrength * wave;

            myCohesionStep[i] = glm::vec2(0); // cohesion removed; zero for debug view

            myXZWaveVelocity[i] += myGradStep[i] + myWaveStep[i];

            float len = glm::length(myXZWaveVelocity[i]);
            if (len > waterSettings.maxVelocity)
                myXZWaveVelocity[i] *= waterSettings.maxVelocity / len;
            myXZWaveVelocity[i] *= waterSettings.waveDamping;
        }
    }
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

void PhysicsEngine::CreateBFSSlopeMap()
{
    static int frameCounter = 0;
    frameCounter++;
    size_t size = CHUNK_SIZE * CHUNK_SIZE;
    for (size_t ii = 0; ii < size; ii++)
    {
        size_t     i             = frameCounter % 2 == 0 ? ii : size - 1 - ii;
        int        x             = i % CHUNK_SIZE;
        int        z             = (i / CHUNK_SIZE) % CHUNK_SIZE;
        glm::ivec2 slopeNeighbor = BreadthFirstSearchSink(x, z);
        myXZBFSSlopeMap[i]       = slopeNeighbor;
    }
}

glm::ivec2 PhysicsEngine::BreadthFirstSearchSink(int x, int z)
{
    static int frameCounter = 0;
    frameCounter++;

    struct Node
    {
        glm::ivec2 position;
        glm::ivec2 firstStepTag;
    };
    constexpr int MAX_RADIUS = 10;
    constexpr int MAX_QUEUE  = (2 * MAX_RADIUS + 1) * (2 * MAX_RADIUS + 1); // radius squared
    Node          queue[MAX_QUEUE];
    int           queueHead = 0, queueTail = 0;
    bool          visited[CHUNK_SIZE * CHUNK_SIZE]{};
    uint8_t       startingHeight = myXZHeightMap[x + z * CHUNK_SIZE];

    // enqueue all neighbors directly next to starting position
    for (int ddz = -1; ddz <= 1; ddz++)
    {
        int dz = frameCounter % 2 == 0 ? ddz : -ddz;
        for (int ddx = -1; ddx <= 1; ddx++)
        {
            int dx = frameCounter % 2 != 0 ? ddx : -ddx;
            if (dx == 0 and dz == 0)
                continue; // skip center

            glm::ivec2 neighborPos = glm::ivec2(x + dx, z + dz);

            if (isOutOfBounds(neighborPos))
                continue;
            uint8_t neighborHeight = myXZHeightMap[neighborPos.x + neighborPos.y * CHUNK_SIZE];

            // only add traversable neighbors as explorable options
            if (neighborHeight >= startingHeight)
                continue;

            // is this a sink?
            if (neighborHeight < startingHeight - 1)
                return glm::ivec2(dx, dz);

            // add as initial node
            queue[queueTail++ % MAX_QUEUE]                      = {neighborPos, glm::ivec2(dx, dz)};
            visited[neighborPos.x + neighborPos.y * CHUNK_SIZE] = true;
        }
    }

    while (queueHead != queueTail)
    {
        Node node = queue[queueHead++ % MAX_QUEUE];

        // for every neighbor node...
        for (int ddz = -1; ddz <= 1; ddz++)
        {
            int dz = frameCounter % 2 != 0 ? ddz : -ddz;
            for (int ddx = -1; ddx <= 1; ddx++)
            {
                int dx = frameCounter % 2 == 0 ? ddx : -ddx;
                if (dx == 0 and dz == 0)
                    continue;

                glm::ivec2 pos = node.position + glm::ivec2{dx, dz};

                if (isOutOfBounds(pos))
                    continue;

                // is this distance too far away to be considered
                float distSq = (pos.x - x) * (pos.x - x) + (pos.y - z) * (pos.y - z);
                if (distSq >= MAX_RADIUS * MAX_RADIUS)
                    continue;

                // already visited? skip
                if (visited[pos.x + pos.y * CHUNK_SIZE])
                    continue;

                uint8_t neighborHeight = myXZHeightMap[pos.x + pos.y * CHUNK_SIZE];

                // not traversable? skip it
                if (neighborHeight >= startingHeight)
                    continue;

                // is it a sink?
                if (neighborHeight < startingHeight - 1)
                {
                    // DONE! Return first step tag
                    return node.firstStepTag;
                }
                else
                {
                    visited[pos.x + pos.y * CHUNK_SIZE] = true;
                    queue[queueTail++ % MAX_QUEUE]      = {pos, node.firstStepTag};
                }
            }
        }
    }

    return glm::ivec2(0);
}

void PhysicsEngine::CreateSlopeMap()
{
    constexpr int R   = 1;
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
