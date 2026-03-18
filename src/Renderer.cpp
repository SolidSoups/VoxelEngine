#include "Renderer.h"

Shader Renderer::myShader;

void Renderer::Initialize(){
  Renderer::myShader = Shader(VERT_PATH, FRAG_PATH); 
}

void Renderer::Destroy(){

}

void Renderer::DrawCube(){
  Renderer::myShader.Bind();
}

