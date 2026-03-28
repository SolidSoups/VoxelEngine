#include "Scene.h"

#include <print>
#include <chrono>

#include "Renderer.h"
#include "VoxelPainter.h"
#include "objects/Mesh.h"
#include "helpers/ChunkMesher.h"
#include "objects/VoxelType.h"
#include "objects/VertexMode.h"

Scene::Scene() {
  VoxelPainter::SetBrushColor(VoxelType_SAND);
  VoxelPainter::PaintSphere(glm::ivec3(16), 15, myVoxelChunk);

  std::println("VoxelCount: {0}", myVoxelChunk.CountNonEmptyVoxels());
}

void Scene::Update(){
  if(myVoxelChunk.isDirty){
    std::println("Chunk is dirty, remeshing");
    auto start = std::chrono::high_resolution_clock::now();
    myChunkMesh = myChunkMesher.CreateMesh_Greedy(myVoxelChunk);
    auto end = std::chrono::high_resolution_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::println("Loading mesh finished. Time: {0:.2f}ms", us / 1000.0);
    myVoxelChunk.isDirty = false;
  }
}

void Scene::Render() {
  FilledVertexMode filledMode;
  WireframeVertexMode wireframeMode;

  glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(4.0f / CHUNK_SIZE));
  Renderer::DrawChunk(myChunkMesh, scale, VoxelType_SAND, filledMode);
  Renderer::DrawChunk(myChunkMesh, scale, VoxelType_WATER, wireframeMode);
}
