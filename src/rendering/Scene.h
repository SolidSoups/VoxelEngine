#pragma once

#include "rendering/meshing/ChunkMesher.h"
#include "voxel/VoxelChunk.h"
#include "voxel/VoxelChunkViews.h"
#include "rendering/Mesh.h"
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
  inline void CopyChunk(const VoxelChunk& chunk) {
    assert(chunk.GetWidth() == myVoxelChunk.GetWidth() && "Copied chunks MUST have the same size");
    myVoxelChunk = chunk;
  }
  inline const VoxelChunkViews& GetChunkViews() const {return myChunkViews; }
  inline Mesh& GetVoxelMesh() { return myChunkMesh; }
  inline MeshStats GetStats() const { return myChunkMesher.latestStats; }

  bool toggleWireframe = false;
private:
  ChunkMesher myChunkMesher;
  VoxelChunk myVoxelChunk{CHUNK_SIZE};
  VoxelChunkViews myChunkViews;
  Mesh myChunkMesh;

};
