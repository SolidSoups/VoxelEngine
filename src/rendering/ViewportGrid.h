#pragma once

#include <vector>

class ViewportGrid{
public:
  ViewportGrid();
  ~ViewportGrid();

  void Draw();
private:
  void GenerateVertices(std::vector<float>& outVertices);
  unsigned int myVAO = 0, myVBO = 0;
  size_t myVertexCount = 0;
};
