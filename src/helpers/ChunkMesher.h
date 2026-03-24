#pragma once

#include <glm/vec3.hpp>

struct VoxelChunk;
struct Mesh;
struct ChunkFaces {
  std::vector<glm::ivec3> tops, bottoms, forwards, backs, lefts, rights;
};

class ChunkMesher {
public:
  // Temporary interface for building a mesh from a chunk
  Mesh TempBuildMesh(const VoxelChunk& aChunk);

private:

  // Takes a chunk as a paramterer, iterates through it's bitset arrays and
  // produces lists containing every position of every directional face.
  // Outputs it in outFaces.
  void CullChunkFaces(const VoxelChunk &aChunk, ChunkFaces& outFaces);

  // Build's a mesh from a list of grid positions for every directional face
  Mesh BuildMeshFromChunkFaces(const ChunkFaces& someChunkFaces);
};
