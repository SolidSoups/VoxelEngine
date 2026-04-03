#include "rendering/Scene.h"

#include <print>
#include <chrono>

#include "rendering/Renderer.h"
#include "voxel/VoxelPainter.h"
#include "rendering/Mesh.h"
#include "rendering/meshing/ChunkMesher.h"
#include "voxel/VoxelType.h"
#include "rendering/NaiveCube.h"
#include "rendering/VertexMode.h"

Scene::Scene() {
  VoxelPainter::SetBrushColor(VoxelType_WATER);
  VoxelPainter::PaintRect(glm::ivec3(0), glm::ivec3(CHUNK_SIZE-1, 10, CHUNK_SIZE-1), myVoxelChunk);
  VoxelPainter::SetBrushColor(VoxelType_SAND);
  VoxelPainter::PaintSphere(glm::ivec3(32), 10, myVoxelChunk);
}

void Scene::Update(){
  if(myVoxelChunk.isDirty){
    myChunkViews.BuildFrom(myVoxelChunk);
    myChunkMesh = myChunkMesher.CreateMesh_Greedy(myChunkViews);
    myVoxelChunk.isDirty = false;
  }
}

void Scene::Render() {
  FilledVertexMode filledMode;
  WireframeVertexMode wireframeMode;

  glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(4.0f / CHUNK_SIZE));
  if(toggleWireframe)
    Renderer::DrawChunk(myChunkMesh, scale, VoxelType_WATER, wireframeMode);
  else
    Renderer::DrawChunk(myChunkMesh, scale, VoxelType_SAND, filledMode);
}
