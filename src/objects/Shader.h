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
public:
  void setVec2(const char *name, const glm::vec2 &value);
  void setVec3(const char *name, const glm::vec3 &value);
  void setVec4(const char *name, const glm::vec4 &value);
  void setBool(const char *name, bool value);
  void setInt(const char *name, int value);
  void setUInt(const char* name, uint32_t value);
  void setFloat(const char *name, float value);
  void setMatrix4(const char *name, const glm::mat4 &value);


private:
  bool myIsValid = false;
  unsigned int myID = 0;
};
