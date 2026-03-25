#pragma once

#include <glad/glad.h>

class IVertexMode {
public:
  virtual ~IVertexMode() = default;
  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
};

class FilledVertexMode : public IVertexMode {
public:
  void OnEnable() override { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
  void OnDisable() override { // noop
  }
};

class WireframeVertexMode : public IVertexMode {
public:
  void OnEnable() override {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
  }
  void OnDisable() override {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_OFFSET_LINE);
  }
};
