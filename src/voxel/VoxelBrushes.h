#pragma once

#include <unordered_map>

class Mesh;

// Pre-generate voxel brushes of different sizes and shapes
// specifically for spheres

class VoxelBrushes{
public:
  // Either gets the vao id of a mesh of the radius,
  // or computes one and caches it
  std::shared_ptr<Mesh> GetSphere(int aRadius);
  bool PrecomputeSphere(int aRadius);

private:
  std::unordered_map<int, std::shared_ptr<Mesh>> mySphereCache;
};
