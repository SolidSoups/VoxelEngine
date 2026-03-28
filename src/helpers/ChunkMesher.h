#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "../objects/VoxelType.h"

struct VoxelChunk;
struct Mesh;
struct ChunkFaces {
  std::vector<glm::ivec3> tops, bottoms, forwards, backs, lefts, rights;
};
struct SliceMask {
  VoxelBitset slices[CHUNK_SIZE * CHUNK_SIZE];
};
enum FaceDirection{
  TOP=0, BOTTOM=1,
  RIGHT=2, LEFT=3,
  FORWARD=4, BACKWARD=5
};
struct GreedyMesh {
  glm::vec3 startPos;
  glm::vec3 endPos;
};

class ChunkMesher {
public: // Culled meshing
  Mesh CreateMesh_Culled(const VoxelChunk &aChunk);
private:
  void CullChunkFaces(const VoxelChunk &aChunk, ChunkFaces &outFaces);
  Mesh BuildMeshFromChunkFaces(const ChunkFaces &someChunkFaces);

public: // Binary Greedy Meshing
  Mesh CreateMesh_Greedy(const VoxelChunk &aChunk);
private:
  void BinaryGreedyMeshFaces(SliceMask &someFaces, FaceDirection aFaceDirection, std::vector<GreedyMesh> &outGreedyMeshes);
  void BuildFaceSlicesForAxis(VoxelBitset *someCells, SliceMask &outPositive,
                       SliceMask &outNegative);
  void BuildGreedyMeshBuffers(
    std::vector<GreedyMesh> &someGreedyMeshes,
    FaceDirection aFaceDirection,
    size_t& aVertexOffset,
    std::vector<float> &outVertices,
    std::vector<unsigned int> &outIndices,
    VoxelType aVoxelType
  );
};
