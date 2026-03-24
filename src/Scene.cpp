#include "Scene.h"

#include <print>
#include <chrono>

#include "Renderer.h"
#include "VoxelPainter.h"
#include "objects/Mesh.h"
#include "helpers/ChunkMesher.h"

Scene::Scene() {
  VoxelPainter::SetBrushColor(VoxelType_WATER);
  VoxelPainter::PaintSphere(glm::vec3(16), 16, voxelChunk);
  // VoxelPainter::PaintRect(glm::ivec3(0), glm::ivec3(31), voxelChunk);
  VoxelPainter::SetBrushColor(VoxelType_SAND);
  VoxelPainter::PaintRect({0,0,0}, {32, 0, 0}, voxelChunk);

  ChunkMesher mesher;

  auto start = std::chrono::high_resolution_clock::now();
  myMesh = mesher.tmp(voxelChunk);
  auto end = std::chrono::high_resolution_clock::now();

  auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::println("TemBuildMesh: {0:.2f}ms", us / 1000.0);
}

void Scene::Render() {
  glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(4.0f / CHUNK_SIZE));
  Renderer::DrawChunk(myMesh, scale, VoxelType_WATER);
}
