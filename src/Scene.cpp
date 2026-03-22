#include "Scene.h"

#include "Renderer.h"
#include "VoxelPainter.h"

Scene::Scene() {
}

void Scene::Render() {
  auto nonEmptyVoxels = voxelChunk.getNonEmpty();

  // Iterate over every non-empty voxel
  const float worldScale = 4.0f / CHUNK_SIZE;
  glm::vec3 color{1.0f};
  for (glm::ivec3 &pos : nonEmptyVoxels) {
    Voxel &voxel = voxelChunk[pos.x, pos.y, pos.z];

    // determine transform
    glm::vec3 position{pos};
    glm::mat4 transform = glm::translate(glm::mat4(1.0), position * worldScale);
    transform = glm::scale(transform, glm::vec3(worldScale));

    Renderer::DrawCube(transform, voxel);
  }
}
