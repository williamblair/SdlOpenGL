#include "Renderer.h"

// static class instance
Renderer Renderer::sInstance;
Renderer::Renderer()
{}

// Initialization
void Renderer::Init( const char *title, const int width, const int height )
{
    // internal 'window should close' boolean
    mQuit = false;

    // init SDL
    if ( SDL_Init(SDL_INIT_VIDEO < 0) ) {
        std::cerr << "Renderer::Renderer: failed to init sdl: " << SDL_GetError() << std::endl;
        exit( EXIT_FAILURE );
    }

    // create the app window
    mWindow = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
    if ( !mWindow ) {
        std::cerr << "Renderer::Renderer: failed to create window: " << SDL_GetError() << std::endl;
        exit( EXIT_FAILURE );
    }
    mWidth = width;
    mHeight = height;

    // set opengl options
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // create a render context
    mContext = SDL_GL_CreateContext( mWindow );

    // enable vsync
    SDL_GL_SetSwapInterval( 1 );

    // init glew
    glewInit();

    mProjMat = glm::perspective( glm::radians(60.0f), float(mWidth)/float(mHeight), 0.1f, 1000.0f );
    mViewMat = glm::mat4( 1.0f );

    mTexturedLitShader = std::make_unique<Shader>(
        "shaders/TexLitVertexShader.glsl",
        "shaders/TexLitFragmentShader.glsl"
    );

    mAmbientLight = glm::vec3(1.0f,1.0f,1.0f);
    for ( int i=0; i<MAX_POS_LIGHTS; ++i) {
        mPosLights[i].position = glm::vec3(0.0f,0.0f,0.0f);
        mPosLights[i].diffuse = glm::vec3(0.0f,0.0f,0.0f);
    }
    for ( int i=0; i<MAX_DIR_LIGHTS; ++i) {
        mDirLights[i].direction = glm::vec3(0.0f,0.0f,0.0f);
        mDirLights[i].diffuse = glm::vec3(0.0f,0.0f,0.0f);
    }
}

Renderer::~Renderer()
{
    SDL_GL_DeleteContext( mContext );
    SDL_DestroyWindow( mWindow );
    SDL_Quit();
}

void Renderer::Clear()
{
    glEnable( GL_DEPTH_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer::SetTexture( const Texture& tex )
{
    glBindTexture( GL_TEXTURE_2D, tex.mTextureID );
}

void Renderer::DrawVertexBuffer( const glm::mat4& modelMat, const VertexBuffer& vb )
{
    glm::mat4 mvpMat = mProjMat * mViewMat * modelMat;
    glm::mat4 normalMat = glm::inverse( glm::transpose( modelMat ));

    // Change shaders if necessary
    switch (vb.mType)
    {
    case VertexBuffer::POS_TEXCOORD:
        if ( mCurShader != mTexturedLitShader.get() ) {
            mCurShader = mTexturedLitShader.get();
            //printf("Set cur shader sPosTexcoordShader\n");
            mCurShader->Use();
        }
        break;
    default:
        std::cerr << "DrawVertexBuffer: unhandled vertex buffer type" << std::endl;
        return;
    }

    mCurShader->SetVec3( "uAmbient", mAmbientLight );
    for ( int i=0; i<MAX_POS_LIGHTS; ++i ) {
        char ufrmName[256];
        sprintf( ufrmName, "uPosLgtPos%d", i );
        mCurShader->SetVec3( ufrmName, mPosLights[i].position );
        sprintf( ufrmName, "uPosLgtDff%d", i );
        mCurShader->SetVec3( ufrmName, mPosLights[i].diffuse );
    }
    for ( int i=0; i<MAX_DIR_LIGHTS; ++i ) {
        char ufrmName[256];
        sprintf( ufrmName, "uDirLgtDir%d", i );
        mCurShader->SetVec3( ufrmName, mDirLights[i].direction );
        sprintf( ufrmName, "uDirLgtDff%d", i );
        mCurShader->SetVec3( ufrmName, mDirLights[i].diffuse );
    }
    mCurShader->SetMat4( "uMvpMatrix", mvpMat );
    mCurShader->SetMat4( "uModelMatrix", modelMat );
    mCurShader->SetMat4( "uNormalMatrix", normalMat );

    glBindVertexArray( vb.mVAO );
    if ( vb.mNumIndices > 0 ) {
        glDrawElements( GL_TRIANGLES, vb.mNumIndices, GL_UNSIGNED_INT, 0 );
    } else {
        glDrawArrays( GL_TRIANGLES, 0, vb.mNumVertices );
    }
}

void Renderer::Update()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e) )
    {
        if ( e.type == SDL_QUIT ) mQuit = true;
    }

    SDL_GL_SwapWindow( mWindow );
}

bool Renderer::ShouldClose()
{
    return mQuit;
}

