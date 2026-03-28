#include "ChunkMesher.h"

#include <utility>
#include <vector>
#include <print>

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

  return Mesh{vertices, indices};
}

std::vector<float> &getFaceVertices(FaceDirection direction) {
  switch (direction) {
  case TOP:
    return ourTopVerts;
  case BOTTOM:
    return ourBottomVerts;
  case RIGHT:
    return ourRightVerts;
  case LEFT:
    return ourLeftVerts;
  case FORWARD:
    return ourForwardVerts;
  case BACKWARD:
    return ourBackVerts;
  }
  std::unreachable(); // inform compiler function will NEVER reach here
}

Mesh ChunkMesher::CreateMesh_Greedy(const VoxelChunk &aChunk) {
  // BUILD FACE SLICES
  SliceMask faces[6];

  // Build slices of faces for each directional face
  BuildFaceSlices(aChunk.zyOccupancy, faces[FaceDirection::RIGHT],
                  faces[FaceDirection::LEFT]);
  BuildFaceSlices(aChunk.xyOccupancy, faces[FaceDirection::BACKWARD],
                  faces[FaceDirection::FORWARD]);
  BuildFaceSlices(aChunk.xzOccupancy, faces[FaceDirection::TOP],
                  faces[FaceDirection::BOTTOM]);

  // GREEDY MESH

  // let's greedy mesh
  FaceDirection direction = FaceDirection::LEFT;
  std::vector<GreedyMesh> greedyMeshes[6];
  size_t greedyMeshCount = 0;
  for (int fd = 0; fd < 6; fd++) {
    BinaryGreedyMeshFaces(faces[fd], (FaceDirection)fd, greedyMeshes[fd]);
    greedyMeshCount += greedyMeshes[fd].size();
  }
  std::println("Built greedy meshes. Resulting faces: {0}, triangles: {1}",
               greedyMeshCount, greedyMeshCount * 2);

  // pre-allocate memory to avoid repeated reallocations
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  vertices.resize(greedyMeshCount * 12);
  indices.resize(greedyMeshCount * 6);

  // Create face meshes for every directional face
  size_t vertexOffset = 0;
  for (int fd = 0; fd < 6; fd++) {
    // Get the face meshes for this direction and the template faces
    std::vector<GreedyMesh> &faceMeshes = greedyMeshes[fd];
    auto &faceTemplateVerts = getFaceVertices((FaceDirection)fd);
    for (size_t i = 0; i < faceMeshes.size(); i++) {
      auto &gMesh = faceMeshes[i];

      // compute scale and center point of rect
      glm::vec3 scale = (gMesh.endPos - gMesh.startPos);
      glm::vec3 center = (gMesh.startPos + gMesh.endPos) / 2.0f;
      // scale calculation gives 0 on the axis that the face
      // is perpendicular to.
      if (scale.x == 0)
        scale.x = 1.f;
      else
        center.x -= 0.5f;
      if (scale.y == 0)
        scale.y = 1.f;
      else
        center.y -= 0.5f;
      if (scale.z == 0)
        scale.z = 1.f;
      else
        center.z -= 0.5f;

      // insert template verticess offset and scaled
      float *v = &vertices[(vertexOffset + i) * 12];
      for (int j = 0; j < 4; j++) {
        v[j * 3 + 0] = faceTemplateVerts[j * 3 + 0] * scale.x + center.x;
        v[j * 3 + 1] = faceTemplateVerts[j * 3 + 1] * scale.y + center.y;
        v[j * 3 + 2] = faceTemplateVerts[j * 3 + 2] * scale.z + center.z;
      }

      // insert quad indices
      unsigned int *idx = &indices[(vertexOffset + i) * 6];
      unsigned int base = (vertexOffset + i) * 4;
      idx[0] = base + 0;
      idx[1] = base + 3;
      idx[2] = base + 1;
      idx[3] = base + 0;
      idx[4] = base + 2;
      idx[5] = base + 3;
    }
    vertexOffset += faceMeshes.size();
  }

  return {vertices, indices};
}

void ChunkMesher::BinaryGreedyMeshFaces(
    SliceMask &someFaces, FaceDirection aFaceDirection,
    std::vector<GreedyMesh> &outGreedyMeshes) {
  for (size_t col = 0; col < CHUNK_SIZE; col++) {
    for (size_t row = 0; row < CHUNK_SIZE; row++) {
      VoxelBitset &aisle = someFaces.slices[col + row * CHUNK_SIZE];

      // The actual binary greedy meshing, find first non-empty
      // cell in aisle and get it's width to create a mask.
      // Then we move down the aisle using the mask to check if
      // the next aisle also contains those cells.
      //
      // We also remember to remove the cells from each aisle matching
      // the mask so we don't reiterate and create duplicate meshes.
      int aisleStart = 0;
      while (aisleStart < CHUNK_SIZE) {
        // Find first non-empty cell in aisle and get width
        aisleStart = std::countr_zero(aisle);
        int aisleWidth = std::countr_one(aisle >> aisleStart);
        if (aisleStart == CHUNK_SIZE) // all zero's, no cells on this aisle
          break;

        // Create mask, avoid undefined behaviour from left-shifting
        // an entire uint32_t
        VoxelBitset mask = (aisleWidth == CHUNK_SIZE)
                               ? ~VoxelBitset(0)
                               : ((VoxelBitset(1) << aisleWidth) - 1)
                                     << aisleStart;
        // remove current voxels so we don't count them again
        aisle &= ~mask;

        // Grow in row-direction until mask doesn't match
        int rowLength = 1;
        while (rowLength + row < CHUNK_SIZE) {
          VoxelBitset &nextXRow =
              someFaces.slices[col + (rowLength + row) * CHUNK_SIZE];
          if ((nextXRow & mask) !=
              mask) // Mask didn't match, we are done growing
            break;

          // remove with mask from next row so we don't duplicate faces
          nextXRow &= ~mask;
          rowLength++;
        }

        // This is the problem!!!! only works for Y-columns
        int aisleEnd = aisleStart + aisleWidth;
        int rowEnd = row + rowLength;
        GreedyMesh newGreedyMesh;
        if (aFaceDirection == FaceDirection::TOP or
            aFaceDirection == FaceDirection::BOTTOM)
          newGreedyMesh =
              GreedyMesh{{aisleStart, col, row}, {aisleEnd, col, rowEnd}};
        else if (aFaceDirection == FaceDirection::RIGHT or
                 aFaceDirection == FaceDirection::LEFT)
          newGreedyMesh =
              GreedyMesh{{col, row, aisleStart}, {col, rowEnd, aisleEnd}};
        else // FaceDirection is FORWARD or BACKWARD
          newGreedyMesh =
              GreedyMesh{{aisleStart, row, col}, {aisleEnd, rowEnd, col}};

        outGreedyMeshes.push_back(newGreedyMesh);
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
  for (int row = 0; row < CHUNK_SIZE; row++) {
    VoxelBitset posBits[CHUNK_SIZE];
    VoxelBitset negBits[CHUNK_SIZE];

    // For this Z layer, gather face masks across all x
    // NOTE: Hacker's delight bit matrix transpose uses MSB, so we need to
    // reverse the order
    for (int column = 0; column < CHUNK_SIZE; column++) {
      VoxelBitset bitset =
          someCells[row * CHUNK_SIZE + CHUNK_SIZE - 1 - column];

      // each bit of posBits[width] is set if
      // the one above is empty (ie its a positive face)
      posBits[column] = bitset & ~(bitset >> 1);
      negBits[column] = bitset & ~(bitset << 1);
    }

    // posBits[x] has bits set along Y
    // we transpose them so posBits[y] has bits set along X instead
    transpose32(posBits);
    transpose32(negBits);

    // NOTE: Hacker's delight bit matrix transpose uses MSB, we use LSB. Reverse
    // order to get correct aisle levels
    for (int aisle = 0; aisle < CHUNK_SIZE; aisle++) {
      outPositive.slices[CHUNK_SIZE - 1 - aisle + row * CHUNK_SIZE] =
          posBits[aisle];
      outNegative.slices[CHUNK_SIZE - 1 - aisle + row * CHUNK_SIZE] =
          negBits[aisle];
    }
  }
}
