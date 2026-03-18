#pragma once

class Shader{
public:
  Shader() = default;
  Shader(const char* aVertPath, const char* aFragPath);
  ~Shader();

  // no copying
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  // allow moving
  Shader(Shader&& other);
  Shader& operator=(Shader&& other);

  void Bind();
  inline bool IsValid() const { return myIsValid; }
  inline unsigned int GetID() const { return myID; }
private:
  bool myIsValid = false;
  unsigned int myID = 0;
};
