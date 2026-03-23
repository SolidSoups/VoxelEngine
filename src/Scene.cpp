#include "Scene.h"

#include <print>
#include "Renderer.h"
#include "VoxelPainter.h"
#include "objects/Mesh.h"
#include "helpers/ChunkMesher.h"

Scene::Scene() {
  VoxelPainter::SetBrushColor(VoxelType_WATER);
  VoxelPainter::PaintSphere(glm::vec3(16), 10, voxelChunk);

  ChunkMesher mesher;

  myMesh = mesher.TempBuildMesh(voxelChunk);
}

void Scene::Render() {
  auto nonEmptyVoxels = voxelChunk.getNonEmpty();

  // Log voxel count
  uint64_t voxelCount = voxelChunk.CountNonEmptyVoxels();
  std::println("Voxels: {0}", voxelCount);

  // Iterate over every non-empty voxel
  // const float worldScale = 4.0f / CHUNK_SIZE;
  // glm::vec3 color{1.0f};
  // for (glm::ivec3 &pos : nonEmptyVoxels) {
  //   Voxel &voxel = voxelChunk.GetVoxel({pos.x, pos.y, pos.z});
  //
  //   // determine transform
  //   glm::vec3 position{pos};
  //   glm::mat4 transform = glm::translate(glm::mat4(1.0), position * worldScale);
  //   transform = glm::scale(transform, glm::vec3(worldScale));
  //
  //   Renderer::DrawCube(transform, voxel);
  // }
  //
  glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(4.0f / CHUNK_SIZE));
  Renderer::DrawChunk(myMesh, scale, VoxelType_WATER);
}
