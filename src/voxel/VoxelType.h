#pragma once

#include <vector>

#include <glm/vec3.hpp>

#define CHUNK_SIZE 64
#define VOXEL_TYPES 3
using VoxelIndex  = uint32_t;
using VoxelBitset = uint64_t;
using Voxel       = uint8_t;
enum VoxelType : uint8_t
{
    VoxelType_EMPTY = 0,
    VoxelType_STONE = 1,
    VoxelType_SAND  = 2,
    VoxelType_WATER = 3,

    VoxelType_PREVIEW,
    VoxelType_MAX
};
constexpr int ourStoneID = 0;
constexpr int ourSandID = 1;
constexpr int ourWaterID = 2;
#define STONE_MASK 0x01
#define SAND_MASK 0x02
#define WATER_MASK 0x03
inline std::vector<const char *> ourVoxelNames  = {"EMPTY", "STONE", "SAND", "WATER"};
inline std::vector<glm::vec3>    ourVoxelColors = {
    {0.0f, 0.0f, 0.0f},      // EMPTY
    {0.53f, 0.55f, 0.55f},   // STONE
    {0.96f, 0.88f, 0.69f},   // SAND
    {0.227f, 0.247f, 0.788f} // WATER
};

// helper functions

inline glm::ivec3 getVoxelGridPosition(VoxelIndex aIndex)
{
    return {aIndex % CHUNK_SIZE, (aIndex / CHUNK_SIZE) % CHUNK_SIZE, aIndex / (CHUNK_SIZE * CHUNK_SIZE)};
}

inline VoxelIndex getVoxelIndex(const glm::ivec3 &aGridPosition)
{
    return aGridPosition.x + aGridPosition.y * CHUNK_SIZE + aGridPosition.z * CHUNK_SIZE * CHUNK_SIZE;
}
inline bool isOutOfBounds(const glm::ivec3& aGridPosition){
    return aGridPosition.x < 0 or aGridPosition.x >= CHUNK_SIZE or
    aGridPosition.y < 0 or aGridPosition.y >= CHUNK_SIZE or
    aGridPosition.z < 0 or aGridPosition.z >= CHUNK_SIZE;
}
inline bool isOutOfBounds(const glm::ivec2& aGridPosition){
    return aGridPosition.x < 0 or aGridPosition.x >= CHUNK_SIZE or
    aGridPosition.y < 0 or aGridPosition.y >= CHUNK_SIZE;
}
