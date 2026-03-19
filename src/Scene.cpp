#include "Scene.h"

#include <cstdlib>

#include "Renderer.h"
#include "objects/Camera.h"

Scene::Scene() {
  // Create an interesting scene

  float radius = (float)CHUNK_SIZE / 2.f;
  glm::vec3 centerPoint{radius};
  for (int z = 0; z < CHUNK_SIZE; z++)
    for (int y = 0; y < CHUNK_SIZE; y++)
      for (int x = 0; x < CHUNK_SIZE; x++) {
        glm::vec3 position{x, y, z};
        float distance = glm::length(position - centerPoint);
        if (distance < radius) {
          voxelChunk[x, y, z] = (int)(rand() % 3);
        }
      }
}

void Scene::Render() {
  auto nonEmptyVoxels = voxelChunk.getNonEmpty();

  // Iterate over every non-empty voxel
  const float worldScale = 4.0f / CHUNK_SIZE;
  glm::vec3 color{1.0f};
  for (glm::ivec3 &pos : nonEmptyVoxels) {
    Voxel &voxel = voxelChunk[pos.x, pos.y, pos.z];

    // cast position to vec3
    glm::vec3 position{pos};

    // determine transform
    glm::mat4 transform = glm::translate(glm::mat4(1.0), position * worldScale);
    transform = glm::scale(transform, glm::vec3(worldScale));

    // determine color
    if (voxel == VoxelType::SAND)
      color = glm::vec3{0.96f, 0.84f, 0.69f};
    else if (voxel == VoxelType::STONE)
      color = glm::vec3{0.53f, 0.55f, 0.55f};

    Renderer::DrawCube(transform, color);
  }
}
