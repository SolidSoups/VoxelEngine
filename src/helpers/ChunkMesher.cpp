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
      outSharedVertices.push_back(aFaceTemplate[i * 3 + 1] + face.y - 0.03f);
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
  // IterateFaces(someChunkFaces.bottoms, ourBottomVerts, vertices,
  // indices,
  //              currentIdx);
  // IterateFaces(someChunkFaces.rights, ourRightVerts, vertices, indices,
  //              currentIdx);
  // IterateFaces(someChunkFaces.lefts, ourLeftVerts, vertices, indices,
  //              currentIdx);
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
  BuildFaceSlices(aChunk.xRows, posXFaces, negXFaces);
  BuildFaceSlices(aChunk.zRows, negZFaces, posZFaces);
  BuildFaceSlices(aChunk.yColumns, posYFaces, negYFaces);

  // let's greedy mesh
  auto greedyMeshes = BinaryGreedyMeshY(posYFaces);
  std::println("Building greedy mesh. Resulting faces: {0}, triangles: {1}",
               greedyMeshes.size(), greedyMeshes.size() * 2);

  // pre-allocate memory to avoid repeated reallocations
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  vertices.resize(greedyMeshes.size() * 12);
  indices.resize(greedyMeshes.size() * 6);

  // create faces from all greedy rects
  for (size_t i = 0; i < greedyMeshes.size(); i++) {
    auto &gMesh = greedyMeshes[i];

    // compute scale and center point of rect
    glm::vec3 scale = (gMesh.endPos - gMesh.startPos);
    scale.y = 1.f;
    glm::vec3 center =
        (gMesh.startPos + gMesh.endPos) / 2.0f + glm::vec3(-0.5f, 0.0f, -0.5f);
    std::println("gMesh.startPos = ({0}, {1}, {2})", gMesh.startPos.x, gMesh.startPos.y, gMesh.startPos.z);
    std::println("gMesh.endPos = ({0}, {1}, {2})", gMesh.endPos.x, gMesh.endPos.y, gMesh.endPos.z);
    std::println("Center is ({0}, {1}, {2})", center.x, center.y, center.z);

    // insert template verticess offset and scaled
    float *v = &vertices[i * 12];
    for (int j = 0; j < 4; j++) {
      v[j * 3 + 0] = ourTopVerts[j * 3 + 0] * scale.x + center.x;
      v[j * 3 + 1] = ourTopVerts[j * 3 + 1] * scale.y + center.y;
      v[j * 3 + 2] = ourTopVerts[j * 3 + 2] * scale.z + center.z;
    }

    // insert general face indices for quad
    unsigned int *idx = &indices[i * 6];
    unsigned int base = i * 4;
    idx[0] = base + 0;
    idx[1] = base + 3;
    idx[2] = base + 1;
    idx[3] = base + 0;
    idx[4] = base + 2;
    idx[5] = base + 3;
  }

  return {vertices, indices};
}

std::vector<GreedyMesh> ChunkMesher::BinaryGreedyMeshY(SliceMask &someYFaces) {
  std::vector<GreedyMesh> greedyMeshes;
  for (size_t iy = 0; iy < CHUNK_SIZE; iy++) {
    for (size_t iz = 0; iz < CHUNK_SIZE; iz++) {
      VoxelBitset &xRow = someYFaces.slices[iy + iz * CHUNK_SIZE];

      // For every chunk of faces, grow along the z
      int x = 0;
      while (x < CHUNK_SIZE) {
        x = std::countr_zero(xRow);
        int width = std::countr_one(xRow >> x);
        if (x == CHUNK_SIZE) // all zero's
          break;

        VoxelBitset mask = (width == CHUNK_SIZE)
                               ? ~VoxelBitset(0)
                               : ((VoxelBitset(1) << width) - 1) << x;
        // remove current voxels so we don't count them again
        xRow &= ~mask;

        int z = 1;
        while (z + iz < CHUNK_SIZE) {
          VoxelBitset &nextXRow = someYFaces.slices[iy + (z + iz) * CHUNK_SIZE];
          if ((nextXRow & mask) != mask)
            break;

          // remove with mask from next row so we don't duplicate faces
          nextXRow &= ~mask;
          z++;
        }

        greedyMeshes.push_back(
            GreedyMesh{{x, iy, iz}, {x + width, iy, iz + z}});
      }
    }
  }
  return greedyMeshes;
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
  for (int row = 0; row < CHUNK_SIZE; row++) {
    VoxelBitset posBits[CHUNK_SIZE];
    VoxelBitset negBits[CHUNK_SIZE];

    // For this Z layer, gather face masks across all x
    // NOTE: Hacker's delight bit matrix transpose uses MSB, so we need to reverse the order
    for (int column = CHUNK_SIZE-1; column >= 0; column--) {
      VoxelBitset bitset = someCells[row * CHUNK_SIZE + column];

      // each bit of posBits[width] is set if
      // the one above is empty (ie its a positive face)
      posBits[column] = bitset & ~(bitset >> 1);
      negBits[column] = bitset & ~(bitset << 1);
    }

    // posBits[x] has bits set along Y
    // we transpose them so posBits[y] has bits set along X instead
    transpose32(posBits);
    transpose32(negBits);

    // NOTE: Hacker's delight bit matrix transpose uses MSB, we use LSB. Reverse order to get
    // correct aisle levels
    for (int aisle = 0; aisle < CHUNK_SIZE; aisle++) {
      outPositive.slices[CHUNK_SIZE-1-aisle + row * CHUNK_SIZE] = posBits[aisle];
      outNegative.slices[CHUNK_SIZE-1-aisle + row * CHUNK_SIZE] = negBits[aisle];
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
