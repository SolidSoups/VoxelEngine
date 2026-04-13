#pragma once

enum class DebugMode : uint8_t {
    None = 0,
    HeightMap = 1,
    SlopeDirection = 2,
    FDASlopeDirection = 3,
    MovedThisFrame = 4,
    ActionMap = 5
};
