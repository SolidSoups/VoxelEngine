#include "rendering/NaiveCube.h"

#include <glad/glad.h>

namespace {
std::vector<float> ourCubeVertices = {
  -0.5f, 0.5f, -0.5f, // upper bottom left
  -0.5f, 0.5f,  0.5f, // upper top left
  0.5f, 0.5f, -0.5f, // upper bottom right
  0.5f, 0.5f,  0.5f, // upper top right

  -0.5f, -0.5f, -0.5f, // lower bottom left
  -0.5f, -0.5f,  0.5f, // lower top left
  0.5f, -0.5f, -0.5f, // lower bottom right
  0.5f, -0.5f,  0.5f // lower top right
};
std::vector<unsigned int> ourCubeIndices = {
  // +Y face
  0, 1, 2,
  1, 3, 2,

  // +Z face
  1, 5, 3,
  5, 7, 3,

  // +X face
  3, 7, 2,
  7, 6, 2,

  // -Z face
  2, 4, 0,
  6, 4, 2,

  // -X face
  0, 4, 1,
  4, 5, 1,

  // -Y face
  5, 4, 7,
  4, 6, 7
};
}

NaiveCube::NaiveCube() {
  glGenVertexArrays(1, &myVAO);
  glBindVertexArray(myVAO);

  // generate buffer 
  glGenBuffers(1, &myVBO);
  glBindBuffer(GL_ARRAY_BUFFER, myVBO);
  glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), ourCubeVertices.data(), GL_STATIC_DRAW);

  // generate element buffer
  glGenBuffers(1, &myEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), ourCubeIndices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
}


NaiveCube::~NaiveCube(){
  if(myVAO) glDeleteVertexArrays(1, &myVAO);
  if(myVBO) glDeleteBuffers(1, &myVBO);
  if(myEBO) glDeleteBuffers(1, &myEBO);
}


void NaiveCube::Draw(){
  glBindVertexArray(myVAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
