#include "rendering/passes/GeometryPass.h"

#include "rendering/Mesh.h"
#include "rendering/VertexMode.h"
#include "rendering/Framebuffer.h"
#include "rendering/Renderer.h"
#include "rendering/Shader.h"
#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "voxel/VoxelType.h"

void GeometryPass::Initialize()
{
    myShader = std::make_unique<Shader>("shaders/geometry.vert", "shaders/geometry.frag");
    myShader->Bind();
    glUniform3fv(glGetUniformLocation(myShader->GetID(), "uVoxelColors"), ourVoxelColors.size(), &ourVoxelColors[0][0]);
    myShader->Unbind();
    
    TextureConfig config;
    config.minFilter = GL_NEAREST;
    config.magFilter = GL_NEAREST;
    myArrowTexture = std::make_unique<Texture>("res/voxel_arrow.png", config);

    // G-Buffer attachments
    framebuffer = Framebuffer(1000,
                              800,
                              {
                                  {GL_RGB16F, GL_RGB, GL_FLOAT},      // position
                                  {GL_RGB16F, GL_RGB, GL_FLOAT},      // normal
                                  {GL_RGBA, GL_RGBA, GL_UNSIGNED_INT} // albedo/color
                              });
}

GeometryPass::~GeometryPass() = default;
void GeometryPass::Execute(RenderpassInfo &someInfo)
{
    myShader->Bind();
    myShader->setMatrix4("uView", someInfo.camera.GetViewMatrix());
    myShader->setMatrix4("uProjection", someInfo.camera.GetProjectionMatrix());

    // enable debug here
    if(someInfo.debugMode != DebugMode::None){
        myShader->setInt("uDebugMode", (int)someInfo.debugMode);

        // bind debug texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, someInfo.debugTexture);
        myShader->setInt("uDebugTex", 0);

        // bind arrow texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, myArrowTexture->id);
        myShader->setInt("uArrowTex", 1);

        // set some very important properties
        myShader->setFloat("uChunkWidth", 4.0f);
        myShader->setInt("uChunkSize", CHUNK_SIZE);
    }
    else{
        myShader->setInt("uDebugMode", 0);
    }
    
    auto &chunk = someInfo.scene.GetVoxelChunk();
    myShader->setMatrix4("uTransform", chunk.GetTransform());
    someInfo.vertexMode.OnEnable();
    someInfo.scene.GetVoxelMesh().Draw();
    someInfo.vertexMode.OnDisable();
}
void DrawChunk(const Mesh &aMesh, const glm::mat4 &aTransform, IVertexMode &aVertexMode) {}
