#ifndef GLTF_MESH_H_INCLUDED
#define GLTF_MESH_H_INCLUDED

#include "cgltf.h"

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
    GLTFMesh() :
        mData( nullptr ),
        mPosition( 0.0f, 0.0f, 0.0f ),
        mRotation( 0.0f ),
        mCurrentFrame( 0 ),
        mNextFrame( 1 ),
        mAnimTime( 0.0f )
    {
    }

    ~GLTFMesh()
    {
        if ( mData ) {
            cgltf_free( mData );
            mData = nullptr;
        }
        //glDeleteVertexArrays( 1, &mVAO );
        //glDeleteBuffers( 1, &mVBO );
    }

    bool Load( const char* filename )
    {
        cgltf_options options;
        memset(&options, 0, sizeof(options));

        cgltf_result result = cgltf_parse_file( &options, filename, &mData );
        if ( result != cgltf_result_success ) {
            printf( "Failed to load gltf %s\n", filename );
            return false;
        }

        result = cgltf_load_buffers( &options, mData, filename );
        if ( result != cgltf_result_success ) {
            cgltf_free( mData );
            printf("Failed to load buffers for gltf %s\n", filename );
            return false;
        }

        result = cgltf_validate( mData );
        if ( result != cgltf_result_success ) {
            cgltf_free( mData );
            printf("Failed to validate file %s\n", filename);
            return false;
        }

        printf( "Animations count: %lu\n", mData->animations_count );
        mAnimations.resize( mData->animations_count );
        for ( size_t i = 0; i < mData->animations_count; ++i )
        {
            loadAnimation( mData->animations[i], mAnimations[i] );
        }
        
        printf( "Nodes count: %lu\n", mData->nodes_count );
        mNodes.resize( mData->nodes_count );
        for ( size_t i = 0; i < mData->nodes_count; ++i )
        {
            // only want to process the root node
            if ( mData->nodes[i].parent == nullptr ) {
                loadNodes( mData->nodes[i], mNodes );
                break;
            }
        }

        /* Generate default skeleton nodes */
        {
            glm::vec4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
            // 3 floats per vec3
            mFrameVertices.resize( mNodes.size() * 3 );
            
            Node& rootNode = mNodes[0];
            glm::mat4 transform = rootNode.toMat4();
            pos = transform * pos;
            float* fPtr = glm::value_ptr( pos );
            
            mFrameVertices[0] = fPtr[0];
            mFrameVertices[1] = fPtr[1];
            mFrameVertices[2] = fPtr[2];
            size_t resIndex = 3;
            
            for ( size_t i = 0; i < rootNode.children.size(); ++i )
            {
                loadSkelVertNode( mNodes[rootNode.children[i]],
                    transform,
                    resIndex );
            }
        }

        return true;
    }
    
    void CreateBuffers( GLuint shaderID )
    {
        mShaderID = shaderID;
        mTransformLoc = glGetUniformLocation( mShaderID, "transform" );
        glGenVertexArrays( 1, &mVAO );
        glGenBuffers( 1, &mVBO );

        const size_t FLOATS_PER_VERTEX = 3;

        glBindVertexArray( mVAO );
            glBindBuffer( GL_ARRAY_BUFFER, mVBO );
            glBufferData( GL_ARRAY_BUFFER, mFrameVertices.size()*sizeof(GLfloat), mFrameVertices.data(), GL_DYNAMIC_DRAW );

            // Vertex Attribute
            // ---------------------------------------------------------------------------
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)0 );
            glEnableVertexAttribArray( 0 ); // location=0
            
            // Color Attribute
            // ---------------------------------------------------------------------------
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
            //glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)) );
            //glEnableVertexAttribArray( 1 ); // location=1

            // Texture Attribute
            // ---------------------------------------------------------------------------
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
            //glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)) );
            //glEnableVertexAttribArray( 2 ); // location=1

            glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    void SetPosition( glm::vec3 pos ) {
        mPosition = pos;
        mTransform = glm::translate( glm::mat4(1.0f), mPosition );
    }

    void Draw( glm::mat4& perspMat )
    {
        mRotation += 0.1f;
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
    float mTotalAnimTime;

    GLuint mVAO, mVBO;
    GLuint mShaderID;
    GLuint mTransformLoc;

    struct Node
    {
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
    std::vector<Animation> mAnimations;
  
    void loadAnimation( cgltf_animation& anim, Animation& result )
    {
        if ( anim.name && strlen( anim.name ) > 0 ) {
            result.name = std::string( anim.name );
        } else {
            result.name = "Unknown";
        }
        
        for ( size_t i = 0; i < anim.channels_count; ++i )
        {
            cgltf_animation_channel& channel = anim.channels[i];
            
            switch ( channel.target_path )
            {
            case cgltf_animation_path_type_translation: loadAnimTranslation( channel, result.transKeyFrames ); break;
            case cgltf_animation_path_type_rotation: loadAnimRotation( channel, result.rotKeyFrames ); break;
            case cgltf_animation_path_type_scale: loadAnimScale( channel, result.scaleKeyFrames ); break;
            case cgltf_animation_path_type_weights: printf( "Unhandled weights target path\n" ); break;
            default:
                break;
            }
        }
    }
    
    void loadAnimTranslation( cgltf_animation_channel& channel, std::vector<KeyFrameTrans>& result )
    {
        std::vector<float> times;
        std::vector<glm::vec3> translations;
        
        loadKeyFrameTimes( channel, times );
        loadKeyFrameVec3s( channel, translations );
        
        if ( times.size() != translations.size() ) {
            printf( "Load anim translation error: times size != translations\n" );
            return;
        }
        
        result.resize( times.size() );
        for ( size_t i = 0; i < result.size(); ++i )
        {
            result[i].time = times[i];
            result[i].translation = translations[i];
        }
    }
    
    void loadAnimRotation( cgltf_animation_channel& channel, std::vector<KeyFrameRot>& result )
    {
        std::vector<float> times;
        std::vector<glm::quat> rotations;
        
        loadKeyFrameTimes( channel, times );
        loadKeyFrameQuats( channel, rotations );
        
        if ( times.size() != rotations.size() ) {
            printf( "Load anim rotations error: times size != rotations\n" );
            return;
        }
        
        result.resize( times.size() );
        for ( size_t i = 0; i < result.size(); ++i )
        {
            result[i].time = times[i];
            result[i].rotation = rotations[i];
        }
    }
    
    void loadAnimScale( cgltf_animation_channel& channel, std::vector<KeyFrameScale>& result )
    {
        std::vector<float> times;
        std::vector<glm::vec3> scales;
        
        loadKeyFrameTimes( channel, times );
        loadKeyFrameVec3s( channel, scales );
        
        if ( times.size() != scales.size() ) {
            printf( "Load anim scales error: times size != scales\n" );
            return;
        }
        
        result.resize( times.size() );
        for ( size_t i = 0; i < result.size(); ++i )
        {
            result[i].time = times[i];
            result[i].scale = scales[i];
        }
    }
    
    void loadKeyFrameTimes( cgltf_animation_channel& channel, std::vector<float>& result )
    {
        cgltf_animation_sampler& sampler = *channel.sampler;
        cgltf_accessor& accessor = *sampler.input;
        
        if ( accessor.component_type != cgltf_component_type_r_32f ||
             accessor.type != cgltf_type_scalar ) {
            printf( "Unhandled keyframe times component type or type\n" );
            return;
        }
        
        cgltf_buffer_view& view = *accessor.buffer_view;
        cgltf_buffer& buffer = *view.buffer;
        
        size_t byteIndex = view.offset;
        unsigned char* byteBuff = (unsigned char*)buffer.data;
        const size_t stride = view.stride == 0 ? accessor.stride : view.stride;
        size_t counter = 0;
        result.resize( accessor.count );
        while ( byteIndex < view.size && counter < accessor.count )
        {
            float* floatPtr = (float*)&byteBuff[byteIndex];
            result[counter] = *floatPtr;
            ++counter;
            byteIndex += stride;
        }
    }
    
    void loadKeyFrameVec3s( cgltf_animation_channel& channel, std::vector<glm::vec3>& result )
    {
        cgltf_animation_sampler& sampler = *channel.sampler;
        cgltf_accessor& accessor = *sampler.output;
        
        if ( accessor.component_type != cgltf_component_type_r_32f ||
             accessor.type != cgltf_type_vec3 ) {
            printf( "Unhandled keyframe vec3s component type or type\n" );
            return;
        }
        
        cgltf_buffer_view& view = *accessor.buffer_view;
        cgltf_buffer& buffer = *view.buffer;
        
        size_t byteIndex = view.offset;
        unsigned char* byteBuff = (unsigned char*)buffer.data;
        const size_t stride = view.stride == 0 ? accessor.stride : view.stride;
        size_t counter = 0;
        result.resize( accessor.count );
        while ( byteIndex < view.size && counter < accessor.count )
        {
            float* floatPtr = (float*)&byteBuff[byteIndex];
            result[counter] = glm::vec3( floatPtr[0], floatPtr[1], floatPtr[2] );
            ++counter;
            byteIndex += stride;
        }
    }
    
    void loadKeyFrameQuats( cgltf_animation_channel& channel, std::vector<glm::quat>& result )
    {
        cgltf_animation_sampler& sampler = *channel.sampler;
        cgltf_accessor& accessor = *sampler.output;
        
        if ( accessor.component_type != cgltf_component_type_r_32f ||
             accessor.type != cgltf_type_vec4 ) {
            printf( "Unhandled keyframe vec4s component type or type\n" );
            return;
        }
        
        cgltf_buffer_view& view = *accessor.buffer_view;
        cgltf_buffer& buffer = *view.buffer;
        
        size_t byteIndex = view.offset;
        unsigned char* byteBuff = (unsigned char*)buffer.data;
        const size_t stride = view.stride == 0 ? accessor.stride : view.stride;
        size_t counter = 0;
        result.resize( accessor.count );
        while ( byteIndex < view.size && counter < accessor.count )
        {
            float* floatPtr = (float*)&byteBuff[byteIndex];
            result[counter] = glm::quat( floatPtr[3], floatPtr[0], floatPtr[1], floatPtr[2] );
            ++counter;
            byteIndex += stride;
        }
    }
    
    void loadNodes( cgltf_node& rootNode, std::vector<Node>& result )
    {
        if ( rootNode.parent != nullptr ) {
            printf( "Error - expected root node to have no parent\n" );
            return;
        }
        
        size_t nodeIndex = 0;
        Node& node = result[0];
        node.parent = -1;
        loadNodeTransforms( rootNode, node );
        int nodeCounter = 0;
        for ( size_t i = 0; i < rootNode.children_count; ++i )
        {
            loadNode( *rootNode.children[i], 0, nodeCounter, result );
        }
    }
    
    void loadNode( cgltf_node& node,
        const int parentIndex,
        int& nodeIndexCounter,
        std::vector<Node>& result )
    {
        ++nodeIndexCounter;
        const int thisNodeIndex = nodeIndexCounter;
        result[parentIndex].children.push_back( thisNodeIndex );
        result[thisNodeIndex].parent = parentIndex;
        loadNodeTransforms( node, result[thisNodeIndex] );
        for ( size_t i = 0; i < node.children_count; ++i )
        {
            loadNode( *node.children[i], thisNodeIndex, nodeIndexCounter, result );
        }
    }
    
    void loadNodeTransforms( cgltf_node& node, Node& result )
    {
        /* Set identity default transforms */
        result.rotation = glm::angleAxis( 0.0f, glm::vec3( 1.0f, 0.0f, 0.0f ) );
        result.translation = glm::vec3( 0.0f, 0.0f, 0.0f );
        result.scale = glm::vec3( 1.0f, 1.0f, 1.0f );
        
        /* Load actual transforms if they are available */
        if ( node.has_translation ) {
            result.translation = glm::vec3(
                node.translation[0],
                node.translation[1],
                node.translation[2]
            );
        }
        if ( node.has_rotation ) {
            result.rotation = glm::quat(
                node.rotation[3], // in glm, w component stored first
                node.rotation[0],
                node.rotation[1],
                node.rotation[2]
            );
        }
        if ( node.has_scale ) {
            result.scale = glm::vec3(
                node.scale[0],
                node.scale[1],
                node.scale[2]
            );
        }
        
        if ( node.has_matrix ) {
            printf("Warning - unhandled node transform matrix\n");
            return;
        }
    }
    
    void loadSkelVertNode( Node& node, glm::mat4& parentTransform, size_t& resIndex )
    {
        glm::vec4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
        glm::mat4 transform = parentTransform * node.toMat4();
        pos = transform * pos;
        float* fPtr = glm::value_ptr( pos );
        mFrameVertices[resIndex+0] = fPtr[0];
        mFrameVertices[resIndex+1] = fPtr[1];
        mFrameVertices[resIndex+2] = fPtr[2];
        resIndex += 3; // 3 floats per vertex
        
        for ( size_t i = 0; i < node.children.size(); ++i )
        {
            loadSkelVertNode( mNodes[node.children[i]],
                transform,
                resIndex );
        }
    }
};

#endif // GLTF_MESH_H_INCLUDED

