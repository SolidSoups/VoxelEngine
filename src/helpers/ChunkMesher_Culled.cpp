#include "ChunkMesher.h"

#include "../objects/VoxelChunk.h"
#include "../objects/Mesh.h"
#include "../objects/CubeFaces.h"


Mesh ChunkMesher::CreateMesh_Culled(const VoxelChunk &aChunk) {
  std::println("Voxel count: {0}", aChunk.CountNonEmptyVoxels());
  ChunkFaces faces;
  CullChunkFaces(aChunk, faces);
  return BuildMeshFromChunkFaces(faces);
}

void ChunkMesher::CullChunkFaces(const VoxelChunk &aChunk,
                                 ChunkFaces &outFaces) {

  const size_t size = CHUNK_SIZE * CHUNK_SIZE;

  // Iterate all right and left faces in x-row
  for (size_t i = 0; i < size; i++) {
    int z = i % CHUNK_SIZE;
    int y = i / CHUNK_SIZE;
    VoxelBitset bitset = aChunk.zyOccupancy[i];

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
    VoxelBitset bitset = aChunk.xyOccupancy[i];

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
    VoxelBitset bitset = aChunk.xzOccupancy[i];

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

void IterateFaces(const std::vector<glm::ivec3> &someFacePositions,
                  const std::vector<float> &aFaceTemplate,
                  std::vector<float> &outSharedVertices,
                  std::vector<unsigned int> &outSharedIndices,
                  unsigned int &currentIndex) {
  for (const glm::ivec3 &face : someFacePositions) {
    for (int i = 0; i < 4; i++) {
      outSharedVertices.push_back(aFaceTemplate[i * 3 + 0] + face.x);
      outSharedVertices.push_back(aFaceTemplate[i * 3 + 1] + face.y);
      outSharedVertices.push_back(aFaceTemplate[i * 3 + 2] + face.z);
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

Mesh ChunkMesher::BuildMeshFromChunkFaces(const ChunkFaces &someChunkFaces) {
  // Print the chunks size, face count and triangle count
  uint64_t facesCount =
      someChunkFaces.forwards.size() + someChunkFaces.backs.size() +
      someChunkFaces.lefts.size() + someChunkFaces.rights.size() +
      someChunkFaces.tops.size() + someChunkFaces.bottoms.size();
  std::println("Building Culled Mesh. Triangle Count: {0}", facesCount * 2);

  // Iterate through all directional faces and build the mesh
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int currentIdx = 0;
  IterateFaces(someChunkFaces.tops, ourTopVerts, vertices, indices, currentIdx);
  IterateFaces(someChunkFaces.bottoms, ourBottomVerts, vertices, indices,
               currentIdx);
  IterateFaces(someChunkFaces.rights, ourRightVerts, vertices, indices,
               currentIdx);
  IterateFaces(someChunkFaces.lefts, ourLeftVerts, vertices, indices,
               currentIdx);
  IterateFaces(someChunkFaces.forwards, ourForwardVerts, vertices, indices,
               currentIdx);
  IterateFaces(someChunkFaces.backs, ourBackVerts, vertices, indices,
               currentIdx);

  Mesh::VertexAttrib attribs[] = {{.index=0, .size=3, .offset=0}};
  return Mesh{vertices, indices, (int)sizeof(float)*3, attribs};
}
