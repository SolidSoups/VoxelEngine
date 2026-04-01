#include "Mesh.h"

#include <print>

#include <glad/glad.h>

// current format: v = [p.x, p.y, p.z];
Mesh::Mesh(const std::vector<float> &someVertices,
           const std::vector<unsigned int> &someIndices,
           int aStride,
           std::span<VertexAttrib> someAttributes)
    : myElementsSize(someIndices.size()) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // generate vertex buffer
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, someVertices.size() * sizeof(float),
               someVertices.data(), GL_STATIC_DRAW);

  // generate element buffer
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               someIndices.size() * sizeof(unsigned int), someIndices.data(),
               GL_STATIC_DRAW);

  for(const auto& attr : someAttributes){
    glVertexAttribPointer(attr.index, attr.size, GL_FLOAT, GL_FALSE,
                          aStride, (void*)(intptr_t)attr.offset);
    glEnableVertexAttribArray(attr.index);
  }
}
Mesh::~Mesh() {
  if (vao)
    glDeleteVertexArrays(1, &vao);
  if (vbo)
    glDeleteBuffers(1, &vbo);
  if (ebo)
    glDeleteBuffers(1, &ebo);
}
Mesh::Mesh(Mesh &&other) noexcept {
  vao = other.vao;
  vbo = other.vbo;
  ebo = other.ebo;
  myElementsSize = other.myElementsSize;
  other.vao = 0;
  other.vbo = 0;
  other.ebo = 0;
}
Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this != &other) {
    if (vao)
      glDeleteVertexArrays(1, &vao);
    if (vbo)
      glDeleteBuffers(1, &vbo);
    if (ebo)
      glDeleteBuffers(1, &ebo);
    vao = other.vao;
    vbo = other.vbo;
    ebo = other.ebo;
    myElementsSize = other.myElementsSize;
    other.vao = 0;
    other.vbo = 0;
    other.ebo = 0;
  }
  return *this;
}

void Mesh::Draw() const {
  if(vao){
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, myElementsSize, GL_UNSIGNED_INT, 0);
  }
  else
    std::println("Vertex Array Object is null!)");
}
