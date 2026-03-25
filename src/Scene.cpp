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
  VoxelPainter::SetBrushColor(VoxelType_WATER);
  VoxelPainter::PaintSphere(glm::vec3(16), 15, voxelChunk);
  ChunkMesher mesher;

  std::println("Loading mesh");
  auto start = std::chrono::high_resolution_clock::now();
  myMesh = mesher.CreateMesh_Greedy(voxelChunk);
  auto end = std::chrono::high_resolution_clock::now();
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::println("Loading mesh finished. Time: {0:.2f}ms", us / 1000.0);

  
  myMesh1 = mesher.CreateMesh_Culled(voxelChunk);
}

void Scene::Render() {
  glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(4.0f / CHUNK_SIZE));


  FilledVertexMode filledMode;
  WireframeVertexMode wireframeMode;
  if(toggleMesh){
    Renderer::DrawChunk(myMesh1, scale, VoxelType_SAND, filledMode);
    Renderer::DrawChunk(myMesh1, scale, VoxelType_WATER, wireframeMode);
  }
  else{
    Renderer::DrawChunk(myMesh, scale, VoxelType_SAND, filledMode);
    Renderer::DrawChunk(myMesh, scale, VoxelType_WATER, wireframeMode);
  }
  

}
