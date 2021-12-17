#ifndef GLTF_MESH_H_INCLUDED
#define GLTF_MESH_H_INCLUDED

#include <map>
#include <cassert>
#include <string>

#include "cgltf.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Animation.h>

class GLTFMesh
{
public:
    GLTFMesh();
    ~GLTFMesh();

    bool Load( const char* filename );
    void CreateBuffers( GLuint shaderID );
    void Update( const float dt );

    void SetPosition( glm::vec3 pos ) {
        mPosition = pos;
        mTransform = glm::translate( glm::mat4(1.0f), mPosition );
    }
    
    void SetAnimation( const std::string& animName ) {
        for ( size_t i = 0; i < mAnimations.size(); ++i ) {
            if ( mAnimations[i].name == animName ) {
                mCurAnim = &mAnimations[i];
                mAnimTime = 0.0f;
                return;
            }
        }
        printf( "Failed to find anim with name %s\n", animName.c_str() );
        mCurAnim = &mAnimations[0];
        mAnimTime = 0.0f;
    }
    void SetAnimation( const int animIndex ) {
        if ( animIndex < 0 || animIndex >= mAnimations.size() ) {
            printf( "Invalid animation index: %d\n", animIndex );
            return;
        }
        mCurAnim = &mAnimations[animIndex];
        mAnimTime = 0.0f;
        return;
    }

    void Draw( glm::mat4& perspMat )
    {
        mRotation += 0.001f;
        // set the transformation matrix value
        mTransform =
            perspMat *
            glm::translate( glm::mat4(1.0f), mPosition ) *
            glm::rotate( glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 1.0f, 0.0f) );
        glUniformMatrix4fv( mTransformLoc, 1, GL_FALSE, glm::value_ptr(mTransform) );

        //glBindTexture( GL_TEXTURE_2D, mTextureID );
        glBindVertexArray( mVAO );
        glDisable( GL_POINT_SMOOTH );
        glPointSize( 4.0f );
        const size_t FLOATS_PER_VERTEX = 3;
        glDrawArrays( /*GL_TRIANGLES*/ GL_POINTS, 0, mFrameVertices.size() / FLOATS_PER_VERTEX );
    }

private:
    cgltf_data* mData;
    glm::vec3 mPosition;
    glm::mat4 mTransform;
    GLfloat mRotation;

    std::vector<float> mVertices;
    std::vector<unsigned short> mIndices;
    std::vector<float> mFrameVertices; // animated data for the current frame

    size_t mCurrentFrame;
    size_t mNextFrame;
    float mAnimTime;

    GLuint mVAO, mVBO;
    GLuint mShaderID;
    GLuint mTransformLoc;

    struct Node
    {
        std::string name;
        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;
        int parent; // index into mNodes
        std::vector<int> children; // indices into mNodes
        
        inline glm::mat4 toMat4()
        {
            return glm::translate( glm::mat4(1.0f), translation ) *
                glm::mat4_cast( rotation ) *
                glm::scale( glm::mat4(1.0f), scale );
        }
    };
    std::vector<Node> mNodes;
    std::map<cgltf_node*, int> mCgltfNodeToIntMap;
    std::vector<Animation> mAnimations;
    Animation* mCurAnim;
  
    void loadAnimation( cgltf_animation& anim, Animation& result );
    void loadAnimTranslation( cgltf_animation_channel& channel, std::vector<KeyFrameTrans>& result );
    void loadAnimRotation( cgltf_animation_channel& channel, std::vector<KeyFrameRot>& result );
    void loadAnimScale( cgltf_animation_channel& channel, std::vector<KeyFrameScale>& result );
    void loadKeyFrameTimes( cgltf_animation_channel& channel, std::vector<float>& result );
    void loadKeyFrameVec3s( cgltf_animation_channel& channel, std::vector<glm::vec3>& result );
    void loadKeyFrameQuats( cgltf_animation_channel& channel, std::vector<glm::quat>& result );
    void loadNodes( cgltf_node& rootNode, std::vector<Node>& result );
    void loadNode( cgltf_node& node,
        const int parentIndex,
        int& nodeIndexCounter,
        std::vector<Node>& result );
    void loadNodeTransforms( cgltf_node& node, Node& result );
    void loadSkelVertNode( const int nodeIndex, glm::mat4& parentTransform, size_t& resIndex );
};

#endif // GLTF_MESH_H_INCLUDED

