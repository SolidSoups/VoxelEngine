#pragma once

struct CubeFace {
  std::vector<float> vertices;
};

// LIKE THIS OKAY?
//  0 ---- 1
//  | \    |
//  |  \   |   Using CCW
//  |   \  |
//  |    \ |
//  2 ---- 3

inline std::vector<unsigned int> ourFaceIndices{
  0, 3, 1,
  0, 2, 3
};

inline std::vector<float> ourForwardVerts = {
  0.5f,  0.5f, -0.5f,
 -0.5f,  0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
 -0.5f, -0.5f, -0.5f,
};

inline std::vector<float> ourBackVerts = {
 -0.5f,  0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
 -0.5f, -0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
};

inline std::vector<float> ourLeftVerts = {
 -0.5f,  0.5f, -0.5f,
 -0.5f,  0.5f,  0.5f,
 -0.5f, -0.5f, -0.5f,
 -0.5f, -0.5f,  0.5f,
};

inline std::vector<float> ourRightVerts = {
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f, -0.5f,
  0.5f, -0.5f,  0.5f,
  0.5f, -0.5f, -0.5f,
};

inline std::vector<float> ourTopVerts = {
 -0.5f, 0.5f, -0.5f,
  0.5f, 0.5f, -0.5f,
 -0.5f, 0.5f,  0.5f,
  0.5f, 0.5f,  0.5f,
};

inline std::vector<float> ourBottomVerts = {
 -0.5f, -0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
 -0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
};
