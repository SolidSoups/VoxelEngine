#include "Shader.h"

#include <vector>
#include <iostream>
#include <print>

#include "../helpers/FileSystem.h"

unsigned int compileShader(const char* someCode, unsigned int type){
  unsigned int shaderID;
  int success;
  char infoLog[512];

  // create shader 
  shaderID = glCreateShader(type);
  glShaderSource(shaderID, 1, &someCode, NULL);

  glCompileShader(shaderID);

  // get error status, log status if failed
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    std::string typeStr = (type == GL_VERTEX_SHADER) ? "vertex": "frag";
    std::println(stderr, "Compiling {0} shader failed! Info Log:\n{1}", typeStr, infoLog);
    return 0;
  }

  return shaderID;
}

Shader::Shader(const char *aVertPath, const char *aFragPath) {
  // Read the files and get the bytes
  std::vector<char> vertBytes;
  std::vector<char> fragBytes;
  if (!FileSystem::ReadFile(aVertPath, vertBytes)) {
    std::println(stderr, "Failed reading shader vert file: {0}", aVertPath);
    return;
  }
  if (!FileSystem::ReadFile(aFragPath, fragBytes)) {
    std::println(stderr, "Failed reading shader frag file: {0}", aFragPath);
    return;
  }
  
  // compile shaders
  unsigned int newVert = compileShader(vertBytes.data(), GL_VERTEX_SHADER);
  unsigned int newFrag = compileShader(fragBytes.data(), GL_FRAGMENT_SHADER);

  // catch any failed shaders
  if(!newVert or !newFrag){
    if(newVert) glDeleteShader(newVert);
    if(newFrag) glDeleteShader(newFrag);
    newVert = 0; newFrag = 0;
    return;
  } 

  // link new program
  unsigned int newProgram = glCreateProgram();
  glAttachShader(newProgram, newVert);
  glAttachShader(newProgram, newFrag);
  glLinkProgram(newProgram);

  // delete shaders, they are linked now
  glDeleteShader(newVert);
  glDeleteShader(newFrag);

  // check if program linking failed
  int success;
  glGetProgramiv(newProgram, GL_LINK_STATUS, &success);
  if(!success){
    char infoLog[512];
    glGetProgramInfoLog(newProgram, 512, NULL, infoLog);
    std::println(stderr, "Shader linking failed! Info Log:\n{0}", infoLog);
    glDeleteProgram(newProgram);
    return;
  }

  myIsValid = true;
}

Shader::~Shader(){
  if(myID)
    glDeleteProgram(myID);
  myID = 0;
  myIsValid = false;
}

Shader::Shader(Shader&& other){
  myID = other.myID;
  myIsValid = other.myIsValid;
  other.myID = 0;
  other.myIsValid = false;
}
Shader& Shader::operator=(Shader&& other){
  myID = other.myID;
  myIsValid = other.myIsValid;
  other.myID = 0;
  other.myIsValid = false;
  return *this;
}



void Shader::Bind(){
  if(myID) glUseProgram(myID);
  else std::print(stderr, "Cannot bind a failed shader");
}
