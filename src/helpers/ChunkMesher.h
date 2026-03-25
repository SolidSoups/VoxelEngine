#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "../objects/VoxelType.h"

struct VoxelChunk;
struct Mesh;
struct GreedyMesh {
  glm::vec3 startPos;
  glm::vec3 endPos;
};
struct ChunkFaces {
  std::vector<glm::ivec3> tops, bottoms, forwards, backs, lefts, rights;
};
struct SliceMask {
  VoxelBitset slices[CHUNK_SIZE * CHUNK_SIZE];
};

class ChunkMesher {
public:
  // Temporary interface for building a mesh from a chunk
  Mesh TempBuildMesh(const VoxelChunk &aChunk);

  Mesh tmp(const VoxelChunk &aChunk);
  std::vector<GreedyMesh> BinaryGreedyMeshY(SliceMask &mask);

private:
  // Takes a chunk as a paramterer, iterates through it's bitset arrays and
  // produces lists containing every position of every directional face.
  // Outputs it in outFaces.
  void CullChunkFaces(const VoxelChunk &aChunk, ChunkFaces &outFaces);

  // Build's a mesh from a list of grid positions for every directional face
  Mesh BuildMeshFromChunkFaces(const ChunkFaces &someChunkFaces);

  // Build positive and negative face slices from a collection of cells
  void BuildFaceSlices(VoxelBitset *someCells, SliceMask &outPositive,
                       SliceMask &outNegative);
};
