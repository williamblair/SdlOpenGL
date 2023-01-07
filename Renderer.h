#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <iostream>
#include <memory>
#include <cstdlib>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#define MAX_POS_LIGHTS 1
#define MAX_DIR_LIGHTS 1

/* Renderer Singleton Class */
class Renderer
{
public:

    static Renderer* GetInstance() { return &sInstance; }

    void Init( const char* title, const int width, const int height );
    ~Renderer();

    // clear the screen
    void Clear();

    // Apply the given texture to all future draw calls
    void SetTexture( const Texture& tex );

    struct PositionalLight
    {
        glm::vec3 position;
        glm::vec3 diffuse;
    };
    //TODO
    //bool SetPosLight( const PositionalLight& lgt, const size_t index );
    
    struct DirectionalLight
    {
        glm::vec3 direction;
        glm::vec3 diffuse;
    };
    //TODO
    bool SetDirLight( const DirectionalLight& lgt, const size_t index );

    //TODO
    //void SetAmbientLight( float r, float g, float b );

    // Render the data of the input vertex buffer with the given model matrix
    void DrawVertexBuffer( const glm::mat4& modelMat, const VertexBuffer& vb );

    // test if the window should close
    bool ShouldClose();

    // Poll events and
    // update the screen
    void Update();


private:

    SDL_Window* mWindow;
    SDL_GLContext mContext;
    int mWidth, mHeight;
    bool mQuit;
    
    glm::mat4 mProjMat; // projection matrix
    glm::mat4 mViewMat; // view/camera matrix

    std::unique_ptr<Shader> mTexturedLitShader;
    Shader* mCurShader;

    glm::vec3 mAmbientLight;
    PositionalLight mPosLights[MAX_POS_LIGHTS];
    DirectionalLight mDirLights[MAX_DIR_LIGHTS];

    // singleton instance and enforced private ctor/copy/assignment
    static Renderer sInstance;
    Renderer();
    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
};

#endif

