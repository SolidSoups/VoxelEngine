#include "ChunkMesher.h"

#include <utility>
#include <vector>
#include <print>

#include "../objects/VoxelChunk.h"
#include "../objects/Mesh.h"
#include "../objects/CubeFaces.h"

namespace {
// Get the vertices for a face
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

// Transposes a 32x32 bitmatrix, ie flips on the diagonal. Uses most significant
// bit as origin. See Hacker's Delight, section 7.3
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

// Transposes a 64x64 bitmatrix.
void transpose64c(uint64_t a[64]) {
  static const struct {
    int shift;
    uint64_t mask;
  } stages[] = {{32, 0x00000000FFFFFFFF}, {16, 0x0000FFFF0000FFFF},
                {8, 0x00FF00FF00FF00FF},  {4, 0x0F0F0F0F0F0F0F0F},
                {2, 0x3333333333333333},  {1, 0x5555555555555555}};

  for (auto [j, m] : stages) {
    for (int k = 0; k < 64; k = ((k | j) + 1) & ~j) {
      uint64_t t = (a[k] ^ (a[k | j] >> j)) & m;
      a[k] ^= t;
      a[k | j] ^= t << j;
    }
  }
}
} // namespace

std::string getTypeStr(size_t aType) {
  switch (aType) {
  case 0:
    return "STONE";
  case 1:
    return "SAND";
  case 2:
    return "WATER";
  }

  return "Unknown";
}

Mesh ChunkMesher::CreateMesh_Greedy(const VoxelChunk &aChunk) {
  // mesh buffers
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  size_t vertexOffset = 0;

  // Greedy mesh on individual block types
  // to allow setting color attributes
  for (int t = 0; t < VOXEL_TYPES; t++) {
    std::string typeStr = getTypeStr(t);
    std::println("Greedy meshing type: {0}", typeStr);

    // Get isolated axis views of the type
    VoxelBitset *zy = aChunk.zyIsolatedVoxels + t * CHUNK_SIZE * CHUNK_SIZE;
    VoxelBitset *xy = aChunk.xyIsolatedVoxels + t * CHUNK_SIZE * CHUNK_SIZE;
    VoxelBitset *xz = aChunk.xzIsolatedVoxels + t * CHUNK_SIZE * CHUNK_SIZE;

    // Store planar slices of every directional face
    // A slice is perpendicular to its direction
    SliceMask faces[6];

    // Build face slices for every axis
    std::println("Building face slices");
    BuildFaceSlicesForAxis(zy, aChunk.zyOccupancy, faces[RIGHT], faces[LEFT]);
    BuildFaceSlicesForAxis(xy, aChunk.xyOccupancy, faces[BACKWARD],
                           faces[FORWARD]);
    BuildFaceSlicesForAxis(xz, aChunk.xzOccupancy, faces[TOP], faces[BOTTOM]);


    std::println("Building greedy meshes for every face");
    std::vector<GreedyMesh> greedyMeshes[6];
    size_t greedyMeshCount = 0;
    // for every face direction...
    for (int fd = 0; fd < 6; fd++) {
      // ... merge faces with binary greedy mesh algorithm
      BinaryGreedyMeshFaces(faces[fd], (FaceDirection)fd, greedyMeshes[fd]);
      greedyMeshCount += greedyMeshes[fd].size();
    }

    // DEBUG
    std::println("{0}: Build greedy mesh. Number of triangles: {1}", typeStr,
                 greedyMeshCount * 2);

    // Resize mesh buffers to avoid repeated allocations
    vertices.resize(vertices.size() + greedyMeshCount * 16);
    indices.resize(indices.size() + greedyMeshCount * 6);

    std::println("Stiching togehter faces");
    // for every face direction...
    VoxelType voxelType = (VoxelType)(t + 1);
    for (int fd = 0; fd < 6; fd++) {
      // ... build vertices and indices
      std::vector<GreedyMesh> &faceMeshes = greedyMeshes[fd];
      BuildGreedyMeshBuffers(faceMeshes, (FaceDirection)fd, vertexOffset,
                             vertices, indices, voxelType);
    }
  }

  // return new mesh
  return {vertices, indices};
}

void ChunkMesher::BuildGreedyMeshBuffers(
    std::vector<GreedyMesh> &someGreedyMeshes, FaceDirection aFaceDirection,
    size_t &aVertexOffset, std::vector<float> &outVertices,
    std::vector<unsigned int> &outIndices, VoxelType aVoxelType) {
  auto &faceTemplateVerts = getFaceVertices(aFaceDirection);
  for (size_t i = 0; i < someGreedyMeshes.size(); i++) {
    auto &gMesh = someGreedyMeshes[i];

    // compute scale and center point of rect
    glm::vec3 scale = (gMesh.endPos - gMesh.startPos);
    glm::vec3 center = (gMesh.startPos + gMesh.endPos) / 2.0f;

    // Perpendicular axis has zero span, so it gets unit scale,
    // while spanning axis get their center shifted by -0.5 to
    // align with voxel grid edges
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
    float *v = &outVertices[(aVertexOffset + i) * 16];
    for (int j = 0; j < 4; j++) {
      v[j * 4 + 0] = faceTemplateVerts[j * 3 + 0] * scale.x + center.x;
      v[j * 4 + 1] = faceTemplateVerts[j * 3 + 1] * scale.y + center.y;
      v[j * 4 + 2] = faceTemplateVerts[j * 3 + 2] * scale.z + center.z;
      v[j * 4 + 3] = (float)aVoxelType;
    }

    // insert quad indices
    unsigned int *idx = &outIndices[(aVertexOffset + i) * 6];
    unsigned int base = (aVertexOffset + i) * 4;
    idx[0] = base + 0;
    idx[1] = base + 3;
    idx[2] = base + 1;
    idx[3] = base + 0;
    idx[4] = base + 2;
    idx[5] = base + 3;
  }
  aVertexOffset += someGreedyMeshes.size();
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

void ChunkMesher::BuildFaceSlicesForAxis(VoxelBitset *somePerTypeCells,
                                         VoxelBitset *someOccupancyCells,
                                         SliceMask &outPositive,
                                         SliceMask &outNegative) {
  for (int row = 0; row < CHUNK_SIZE; row++) {
    VoxelBitset posBits[CHUNK_SIZE];
    VoxelBitset negBits[CHUNK_SIZE];

    // For this Z layer, gather face masks across all x
    // NOTE: Hacker's delight bit matrix transpose uses MSB, so we need to
    // reverse the order
    for (int column = 0; column < CHUNK_SIZE; column++) {
      VoxelBitset typeBitset =
          somePerTypeCells[CHUNK_SIZE - 1 - column + row * CHUNK_SIZE];
      VoxelBitset allBitset =
          someOccupancyCells[CHUNK_SIZE - 1 - column + row * CHUNK_SIZE];

      // Type is present at this cell, and nothing
      // is above, or below
      posBits[column] = typeBitset & ~(allBitset >> VoxelBitset(1));
      negBits[column] = typeBitset & ~(allBitset << VoxelBitset(1));
    }

    // posBits[x] has bits set along Y
    // we transpose them so posBits[y] has bits set along X instead
    transpose64c(posBits);
    transpose64c(negBits);

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
