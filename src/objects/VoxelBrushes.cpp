#include "VoxelBrushes.h"

#include <print>

#include "Mesh.h"
#include "VoxelType.h"
#include "../helpers/ChunkMesher.h"

std::shared_ptr<Mesh> VoxelBrushes::GetSphere(int aRadius) {
  // check if sphere has been precomputed
  auto it = mySphereCache.find(aRadius);
  if (it != mySphereCache.end()) {
    return it->second;
  }

  if(PrecomputeSphere(aRadius))
    return mySphereCache[aRadius];

  // failed, or wrong radius
  return nullptr;
}

bool VoxelBrushes::PrecomputeSphere(int aRadius) {
  if(aRadius >= CHUNK_SIZE){
    std::println(stderr, "Cannot generate a sphere with a radius larger than CHUNK_SIZE={0}", CHUNK_SIZE);
    return false;
  }

  // create bitset grid for sphere
  VoxelBitset xzBitset[CHUNK_SIZE * CHUNK_SIZE]{};
  VoxelBitset xyBitset[CHUNK_SIZE * CHUNK_SIZE]{};
  VoxelBitset zyBitset[CHUNK_SIZE * CHUNK_SIZE]{};

  const float sqRadius = aRadius * aRadius;
  for (int zz = -aRadius; zz <= aRadius; zz++) {
    int z = aRadius + zz;
    for (int yy = -aRadius; yy <= aRadius; yy++) {
      int y = aRadius + yy;
      for (int xx = -aRadius; xx <= aRadius; xx++) {
        int x = aRadius + xx;
        int hypSq = xx*xx + yy*yy + zz*zz;
        if(hypSq <= sqRadius){
          zyBitset[z + y * CHUNK_SIZE] |= (VoxelBitset(1) << x);
          xzBitset[x + z * CHUNK_SIZE] |= (VoxelBitset(1) << y);
          xyBitset[x + y * CHUNK_SIZE] |= (VoxelBitset(1) << z);
        }
      }
    }
  }

  glm::vec3 center(aRadius );
  ChunkMesher mesher;
  Mesh newMesh = mesher.CreateGreedyMesh_FromBitsets(xzBitset, xyBitset, zyBitset, center);
  mySphereCache[aRadius] = std::make_shared<Mesh>(std::move(newMesh));
  return true;
}
