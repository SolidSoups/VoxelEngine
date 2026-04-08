#pragma once

#include "voxel/VoxelType.h"
#include "voxel/VoxelChunk.h"

struct VoxelChunkViews
{
    // Occupancy (0 = empty)
    VoxelBitset *zyOccupancy;
    VoxelBitset *xzOccupancy;
    VoxelBitset *xyOccupancy;

    // isolated views, indexed with type as well

    // indexed by [x + y * CHUNK_SIZE + voxel_id * VOXEL_TYPES]
    VoxelBitset *xyIsolatedVoxels;
    // indexed by [z + z * CHUNK_SIZE + voxel_id * VOXEL_TYPES]
    VoxelBitset *zyIsolatedVoxels;
    // indexed by [x + z * CHUNK_SIZE + voxel_id * VOXEL_TYPES]
    VoxelBitset *xzIsolatedVoxels;

    VoxelChunkViews()
    {
        zyOccupancy      = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]{};
        xzOccupancy      = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]{};
        xyOccupancy      = new VoxelBitset[CHUNK_SIZE * CHUNK_SIZE]{};
        xyIsolatedVoxels = new VoxelBitset[VOXEL_TYPES * CHUNK_SIZE * CHUNK_SIZE]{};
        zyIsolatedVoxels = new VoxelBitset[VOXEL_TYPES * CHUNK_SIZE * CHUNK_SIZE]{};
        xzIsolatedVoxels = new VoxelBitset[VOXEL_TYPES * CHUNK_SIZE * CHUNK_SIZE]{};
    }
    ~VoxelChunkViews()
    {
        delete[] zyOccupancy;
        delete[] xzOccupancy;
        delete[] xyOccupancy;
        delete[] xyIsolatedVoxels;
        delete[] zyIsolatedVoxels;
        delete[] xzIsolatedVoxels;
    }

    inline uint64_t CountNonEmptyVoxels() const
    {
        uint64_t count = 0;
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
            count += std::popcount(xzOccupancy[i]);
        return count;
    }
    inline uint64_t CountVoxelType(VoxelType aType) const
    {
        int          idx   = (int) aType - 1;
        uint64_t     count = 0;
        VoxelBitset *xz    = &xzIsolatedVoxels[idx * CHUNK_SIZE * CHUNK_SIZE];
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
            count += std::popcount(xz[i]);
        return count;
    }

    inline void BuildFrom(VoxelChunk &aChunk)
    {
        constexpr int    TypeStride = CHUNK_SIZE * CHUNK_SIZE;
        constexpr size_t SliceBytes = sizeof(VoxelBitset) * CHUNK_SIZE * CHUNK_SIZE;

        // first clear containers
        memset(zyOccupancy, 0, SliceBytes);
        memset(xzOccupancy, 0, SliceBytes);
        memset(xyOccupancy, 0, SliceBytes);
        memset(zyIsolatedVoxels, 0, SliceBytes * VOXEL_TYPES);
        memset(xzIsolatedVoxels, 0, SliceBytes * VOXEL_TYPES);
        memset(xyIsolatedVoxels, 0, SliceBytes * VOXEL_TYPES);

        size_t width = aChunk.GetWidth();
        for (int i = 0; i < width * width * width; i++)
        {
            VoxelType type = (VoxelType) aChunk[i];
            if (type == VoxelType_EMPTY)
                continue;

            // get coordinates
            int x = i % width;
            int y = (i / width) % width;
            int z = i / (width * width);

            // set cell to occupied
            zyOccupancy[z + y * CHUNK_SIZE] |= (VoxelBitset(1) << x);
            xzOccupancy[x + z * CHUNK_SIZE] |= (VoxelBitset(1) << y);
            xyOccupancy[x + y * CHUNK_SIZE] |= (VoxelBitset(1) << z);

            // set type to occupied
            int typeIdx = (int) type - 1;
            xyIsolatedVoxels[x + y * CHUNK_SIZE + typeIdx * TypeStride] |= (VoxelBitset(1) << z);
            zyIsolatedVoxels[z + y * CHUNK_SIZE + typeIdx * TypeStride] |= (VoxelBitset(1) << x);
            xzIsolatedVoxels[x + z * CHUNK_SIZE + typeIdx * TypeStride] |= (VoxelBitset(1) << y);
        }
    }
};
