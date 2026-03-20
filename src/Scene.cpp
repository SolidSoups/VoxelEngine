#include "Scene.h"

#include <cstdlib>

#include "Renderer.h"
#include "objects/Camera.h"
#include "VoxelPainter.h"

Scene::Scene() {
  // Create an interesting scene
  VoxelPainter::SetBrushColor(VoxelType_STONE);
  VoxelPainter::PaintRect({2, 2, 2}, {3, 8, 4}, voxelChunk);
  VoxelPainter::SetBrushColor(VoxelType_SAND);
  VoxelPainter::PaintVoxel({0, 0, 0}, voxelChunk);
  VoxelPainter::PaintVoxel({0, 0, CHUNK_SIZE-1}, voxelChunk);
  VoxelPainter::PaintVoxel({CHUNK_SIZE-1, 0, 0}, voxelChunk);
  VoxelPainter::PaintVoxel({CHUNK_SIZE-1, 0, CHUNK_SIZE-1}, voxelChunk);
  VoxelPainter::PaintVoxel({0, CHUNK_SIZE-1, 0}, voxelChunk);
  VoxelPainter::PaintVoxel({0, CHUNK_SIZE-1, CHUNK_SIZE-1}, voxelChunk);
  VoxelPainter::PaintVoxel({CHUNK_SIZE-1, CHUNK_SIZE-1, 0}, voxelChunk);
  VoxelPainter::PaintVoxel({CHUNK_SIZE-1, CHUNK_SIZE-1, CHUNK_SIZE-1}, voxelChunk);
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
    if (voxel == VoxelType::VoxelType_SAND)
      color = glm::vec3{0.96f, 0.84f, 0.69f};
    else if (voxel == VoxelType::VoxelType_STONE)
      color = glm::vec3{0.53f, 0.55f, 0.55f};

    Renderer::DrawCube(transform, color);
  }
}
