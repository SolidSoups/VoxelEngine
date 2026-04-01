#pragma once


struct Mesh {
  unsigned int vao = 0;
  unsigned int vbo = 0;
  unsigned int ebo = 0;

  struct VertexAttrib{
    int index;
    int size;
    int offset;
  };

public:
  Mesh() = default;
  Mesh(const std::vector<float> &someVertices,
       const std::vector<unsigned int> &someIndices,
       int aStride,
       std::span<VertexAttrib> someAttributes);
  ~Mesh();

  // Delete copy
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  // Define move
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

public:
  void Draw() const;


private:
  size_t myElementsSize;
};
