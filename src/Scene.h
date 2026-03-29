#pragma once

#include "helpers/ChunkMesher.h"
#include "objects/VoxelChunk.h"
#include "objects/Mesh.h"
struct Camera;

class Scene{
public:
  Scene();

  // delete copy
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  // move
  Scene(Scene&& other) noexcept 
  : myVoxelChunk(std::move(other.myVoxelChunk))
  {}
  Scene& operator=(Scene&& other) noexcept {
    if(this != &other){
      myVoxelChunk = std::move(other.myVoxelChunk);
    }
    return *this;
  }

  void Render();
  void Update();
  inline VoxelChunk& GetVoxelChunk() { return myVoxelChunk; }
  inline Mesh& GetVoxelMesh() { return myChunkMesh; }

  bool toggleWireframe = false;
private:
  ChunkMesher myChunkMesher;
  VoxelChunk myVoxelChunk;
  Mesh myChunkMesh;
};
