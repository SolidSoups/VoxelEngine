#include "ChunkMesher.h"

#include <vector>

#include "../objects/VoxelChunk.h"
#include "../objects/Mesh.h"
#include "../objects/CubeFaces.h"

void IterateFaces(const std::vector<glm::ivec3> &someFacePositions,
                  const std::vector<float> &someFaceTemplate,
                  std::vector<float> &outSharedVertices,
                  std::vector<unsigned int> &outSharedIndices,
                  unsigned int &currentIndex) {
  for (const glm::ivec3 &topFace : someFacePositions) {
    for (int i = 0; i < 4; i++) {
      outSharedVertices.push_back(someFaceTemplate[i * 3 + 0] + topFace.x);
      outSharedVertices.push_back(someFaceTemplate[i * 3 + 1] + topFace.y);
      outSharedVertices.push_back(someFaceTemplate[i * 3 + 2] + topFace.z);
    }

    outSharedIndices.push_back(currentIndex + 0);
    outSharedIndices.push_back(currentIndex + 3);
    outSharedIndices.push_back(currentIndex + 1);
    outSharedIndices.push_back(currentIndex + 0);
    outSharedIndices.push_back(currentIndex + 2);
    outSharedIndices.push_back(currentIndex + 3);
    currentIndex += 4;
  }
}

Mesh ChunkMesher::TempBuildMesh(const VoxelChunk &aChunk) {
  ChunkFaces faces;

  // Cull the faces
  CullChunkFaces(aChunk, faces);

  // iterate through all faces, add to chunk faces
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int currentIdx = 0;
  IterateFaces(faces.tops, ourTopVerts, vertices, indices, currentIdx);
  IterateFaces(faces.bottoms, ourBottomVerts, vertices, indices, currentIdx);
  IterateFaces(faces.rights, ourRightVerts, vertices, indices, currentIdx);
  IterateFaces(faces.lefts, ourLeftVerts, vertices, indices, currentIdx);
  IterateFaces(faces.forwards, ourForwardVerts, vertices, indices, currentIdx);
  IterateFaces(faces.backs, ourBackVerts, vertices, indices, currentIdx);

  // create the mesh object
  return {vertices, indices};
}

void ChunkMesher::CullChunkFaces(const VoxelChunk &aChunk,
                                 ChunkFaces &outFaces) {

  const size_t size = CHUNK_SIZE * CHUNK_SIZE;

  // Iterate all right and left faces in x-row
  for (size_t i = 0; i < size; i++) {
    int z = i % CHUNK_SIZE;
    int y = i / CHUNK_SIZE;
    VoxelBitset bitset = aChunk.xRows[i];

    VoxelBitset pos = bitset & ~(bitset >> 1);
    while (pos) {
      outFaces.rights.push_back({std::countr_zero(pos), y,
                                 z}); // count zeros from least significant bit
      pos &= pos - 1;                 // remove least significant 1 bit
    }
    VoxelBitset neg = bitset & ~(bitset << 1);
    while (neg) {
      outFaces.lefts.push_back({std::countr_zero(neg), y,
                                z}); // count zeros from least significant bit
      neg &= neg - 1;                // remove least significant 1 bit
    }
  }

  // Iterate all forward and backward faces in z-row
  for (size_t i = 0; i < size; i++) {
    int x = i % CHUNK_SIZE;
    int y = i / CHUNK_SIZE;
    VoxelBitset bitset = aChunk.zRows[i];

    VoxelBitset pos = bitset & ~(bitset >> 1);
    while (pos) {
      outFaces.backs.push_back(
          {x, y,
           std::countr_zero(pos)}); // count zeros from least significant bit
      pos &= pos - 1;               // remove least significant 1 bit
    }
    VoxelBitset neg = bitset & ~(bitset << 1);
    while (neg) {
      outFaces.forwards.push_back(
          {x, y,
           std::countr_zero(neg)}); // count zeros from least significant bit
      neg &= neg - 1;               // remove least significant 1 bit
    }
  }

  // Iterate all top and bottom faces in y-column
  for (size_t i = 0; i < size; i++) {
    int x = i % CHUNK_SIZE;
    int z = i / CHUNK_SIZE;
    VoxelBitset bitset = aChunk.yColumns[i];

    VoxelBitset pos = bitset & ~(bitset >> 1);
    while (pos) {
      outFaces.tops.push_back({x, std::countr_zero(pos),
                               z}); // count zeros from least significant bit
      pos &= pos - 1;               // remove least significant 1 bit
    }
    VoxelBitset neg = bitset & ~(bitset << 1);
    while (neg) {
      outFaces.bottoms.push_back({x, std::countr_zero(neg),
                                  z}); // count zeros from least significant bit
      neg &= neg - 1;                  // remove least significant 1 bit
    }
  }
}
