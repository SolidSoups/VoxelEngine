#include "Scene.h"

#include "Renderer.h"

Scene::Scene() {
  // noop

  int y = 0;
  for (int z = 0; z < CHUNK_SIZE; z++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      voxelChunk[x, y, z] = VoxelType::SAND;
    }
  }
  voxelChunk[1, 1, 1] = VoxelType::STONE;
  voxelChunk[2, 1, 1] = VoxelType::STONE;
  voxelChunk[1, 1, 2] = VoxelType::STONE;
  voxelChunk[2, 1, 2] = VoxelType::STONE;
}

void Scene::Render(){
  auto nonEmptyVoxels = voxelChunk.getNonEmpty();

  glm::vec3 color{1.0f};
  for(glm::ivec3& pos : nonEmptyVoxels){
    Voxel& voxel = voxelChunk[pos.x, pos.y, pos.z];
    if(voxel == VoxelType::SAND)
      color = glm::vec3{0.96f, 0.84f, 0.69f};
    else if(voxel == VoxelType::STONE)
      color = glm::vec3{0.53f, 0.55f, 0.55f};

    Renderer::DrawCube(pos, color); 
  }
}
