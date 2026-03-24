#include "ChunkMesher.h"

#include <bitset>
#include <vector>
#include <print>

#include "../objects/VoxelChunk.h"
#include "../objects/Mesh.h"
#include "../objects/CubeFaces.h"

Mesh ChunkMesher::TempBuildMesh(const VoxelChunk &aChunk) {
  std::println("Voxel count: {0}", aChunk.CountNonEmptyVoxels());
  ChunkFaces faces;
  CullChunkFaces(aChunk, faces);
  return BuildMeshFromChunkFaces(faces);
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
  std::println("Building Mesh. Triangle Count: {0}", facesCount * 2);

  // Iterate through all directional faces and build the mesh
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int currentIdx = 0;
  // IterateFaces(someChunkFaces.tops, ourTopVerts, vertices, indices, currentIdx);
  // IterateFaces(someChunkFaces.bottoms, ourBottomVerts, vertices, indices,
  //              currentIdx);
  IterateFaces(someChunkFaces.rights, ourRightVerts, vertices, indices,
               currentIdx);
  IterateFaces(someChunkFaces.lefts, ourLeftVerts, vertices, indices,
               currentIdx);
  // IterateFaces(someChunkFaces.forwards, ourForwardVerts, vertices, indices,
  //              currentIdx);
  // IterateFaces(someChunkFaces.backs, ourBackVerts, vertices, indices,
  //              currentIdx);

  return Mesh{vertices, indices};
}

Mesh ChunkMesher::tmp(const VoxelChunk &aChunk) {
  // x = yFace[x + z * CHUNK_SIZE]
  // y = zFace[y + z * CHUNK_SIZE]
  // z = xFace[z + x * CHUNK_SIZE]
  SliceMask posYFaces, negYFaces, posXFaces, negXFaces, posZFaces, negZFaces;

  // Build slices of faces for each directional face
  BuildFaceSlices(aChunk.xRows, negXFaces, posXFaces);
  BuildFaceSlices(aChunk.zRows, negZFaces, posZFaces);
  BuildFaceSlices(aChunk.yColumns, negYFaces, posYFaces);

  // let's greedy mesh
  BinaryGreedyMeshY(posYFaces); 

  return {};
}

void ChunkMesher::BinaryGreedyMeshY(SliceMask& mask){
  for(size_t y = 0; y < CHUNK_SIZE; y++){
    for(size_t z=0; z < CHUNK_SIZE; z++){
      VoxelBitset xRow = mask.slices[y + z * CHUNK_SIZE];
      
      while(xRow){
        size_t start = std::countr_zero(xRow);
        if(start == CHUNK_SIZE) continue; // no bits to work with
        size_t width = std::countr_one(xRow >> start);

        VoxelBitset mask = ((1u << width) - 1) << start;
        size_t length = 1;

        size_t iz = z;
        // increase z until mask doesnt
        while(true){

        }
      }
    }
  }
}

// Transposes a 32x32 bitmatrix, ie switches the rows and columns
// See Hacker's Delight, section 7.3
void transpose32(uint32_t a[32]) {
  static const struct {
    int shift;
    uint32_t mask;
  } stages[] = {{16, 0x0000FFFF},
                {8, 0x00FF00FF},
                {4, 0x0F0F0F0F},
                {2, 0x33333333},
                {1, 0x55555555}};

  for (auto [j, m] : stages) {
    for (int k = 0; k < 32; k = ((k | j) + 1) & ~j) {
      uint32_t t = (a[k] ^ (a[k | j] >> j)) & m;
      a[k] ^= t;
      a[k | j] ^= t << j;
    }
  }
}

void ChunkMesher::BuildFaceSlices(VoxelBitset *someCells,
                                  SliceMask &outPositive,
                                  SliceMask &outNegative) {
  for (int length = 0; length < CHUNK_SIZE; length++) {
    VoxelBitset posBits[CHUNK_SIZE];
    VoxelBitset negBits[CHUNK_SIZE];

    // For this Z layer, gather face masks across all x
    for (int width = 0; width < CHUNK_SIZE; width++) {
      VoxelBitset bitset = someCells[length * CHUNK_SIZE + width];

      // each bit of posBits[width] is set if
      // the one above is empty (ie its a positive face)
      posBits[width] = bitset & ~(bitset >> 1);
      negBits[width] = bitset & ~(bitset << 1);
    }

    // posBits[x] has bits set along Y
    // we transpose them so posBits[y] has bits set along X instead
    transpose32(posBits);
    transpose32(negBits);

    for (int depth = 0; depth < CHUNK_SIZE; depth++) {
      outPositive.slices[depth + length * CHUNK_SIZE] = posBits[depth];
      outNegative.slices[depth + length * CHUNK_SIZE] = negBits[depth];
    }
  }
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
