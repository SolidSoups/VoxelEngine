#include "ViewportGrid.h"

#include <glm/glm.hpp>

#include "VoxelType.h"

ViewportGrid::ViewportGrid() {
  // v = p.x, p.y, p.z, n.x, n.y, n.z
  // v1 -> v2 => a line
  std::vector<float> vertices;
  GenerateVertices(vertices);
  myVertexCount = vertices.size() / 6;

  glGenVertexArrays(1, &myVAO);
  glBindVertexArray(myVAO);

  // generate buffer for vertices
  glGenBuffers(1, &myVBO);
  glBindBuffer(GL_ARRAY_BUFFER, myVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  // generate attribute for position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // generate attribute for normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

ViewportGrid::~ViewportGrid() {
  if (myVAO)
    glDeleteVertexArrays(1, &myVAO);
  if (myVBO)
    glDeleteBuffers(1, &myVBO);
}

void ViewportGrid::Draw() {
  glBindVertexArray(myVAO);
  glDrawArrays(GL_LINES, 0, myVertexCount);
}

void ViewportGrid::GenerateVertices(std::vector<float> &outVertices) {
  // define attributes for each quad
  struct QuadData {
    glm::vec3 normal;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 origin;
  };
  glm::vec3 x{1.0f, 0.0f, 0.0f};
  glm::vec3 y{0.0f, 1.0f, 0.0f};
  glm::vec3 z{0.0f, 0.0f, -1.0f};
  const float quadLength = 4.0f;
  const float halfVoxelLength = (quadLength / CHUNK_SIZE) * 0.5f;
  glm::vec3 origin{-halfVoxelLength};
  std::vector<QuadData> quads = {
      {-z, +y, +x, origin},
      {-x, +y, -z, origin + x * quadLength},
      {+z, +y, -x, origin + x * quadLength - z * quadLength},
      {+x, +y, +z, origin - z * quadLength},
      {+y, -z, +x, origin},
      {-y, -z, +x, origin + y * quadLength}};

  // helper for pushing vertices into the output vertices arg
  auto pushVert = [&](const glm::vec3 &pos, const glm::vec3 &norm) {
    outVertices.insert(outVertices.end(),
                       {pos.x, pos.y, pos.z, norm.x, norm.y, norm.z});
  };

  // generate lines for each quad
  const float sideLength = 4.0f;
  const float voxelLength = sideLength / CHUNK_SIZE;
  for (QuadData &data : quads) {
    for (int i = 0; i <= CHUNK_SIZE; i++) {
      float vertexLength = voxelLength * i;

      // calculate horizontal and vertical line points
      glm::vec3 horizontalLineA = data.origin + data.up * vertexLength;
      glm::vec3 horizontalLineB = horizontalLineA + data.right * sideLength;
      glm::vec3 verticalLineA = data.origin + data.right * vertexLength;
      glm::vec3 verticalLineB = verticalLineA + data.up * sideLength;

      // push into vertices
      pushVert(horizontalLineA, data.normal);
      pushVert(horizontalLineB, data.normal);
      pushVert(verticalLineA, data.normal);
      pushVert(verticalLineB, data.normal);
    }
  }
}
