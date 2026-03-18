#pragma once

class NaiveCube{
public:
  NaiveCube();
  ~NaiveCube();

  void Draw();

private:
  unsigned int myVAO = 0;
  unsigned int myVBO = 0;
  unsigned int myEBO = 0;
};
