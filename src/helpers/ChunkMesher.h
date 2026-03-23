#pragma once

#include <glm/vec3.hpp>
#include "../objects/VoxelType.h"

struct VoxelChunk;
struct Mesh;
struct ChunkFaces {
  std::vector<glm::ivec3> tops, bottoms, forwards, backs, lefts, rights;
};

class ChunkMesher {
public:
  void CullChunkFaces(const VoxelChunk &aChunk, ChunkFaces& outFaces);
  Mesh TempBuildMesh(const VoxelChunk& aChunk);
};
