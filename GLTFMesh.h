#ifndef GLTF_MESH_H_INCLUDED
#define GLTF_MESH_H_INCLUDED

#include "cgltf.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class GLTFMesh
{
public:
    GLTFMesh() :
        mData( nullptr ),
        mPosition( 0.0f, 0.0f, 0.0f ),
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
        glDeleteVertexArrays( 1, &mVAO );
        glDeleteBuffers( 1, &mVBO );
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
        for ( size_t i = 0; i < mData->animations_count; ++i )
        {
            handleAnimation( &mData->animations[i] );
        }

        printf( "Vertices:\n" );
        for ( size_t i = 0; i < mVertices.size(); i += 3 )
        {
            printf( "%f, %f, %f\n",
                    mVertices[i+0],
                    mVertices[i+1],
                    mVertices[i+2] );
        }
        printf( "Indices:\n" );
        for ( size_t i = 0; i < mIndices.size() / 3; i += 3 )
        {
            printf( "%u, %u, %u\n",
                    mIndices[i+0],
                    mIndices[i+1],
                    mIndices[i+2] );
        }
        printf( "KeyFrames:\n" );
        for ( KeyFrame& f : mKeyFrames )
        {
            f.print();
        }

        return true;
    }

    void CreateBuffer( GLuint shaderID )
    {
        mShaderID = shaderID;
        mTransformLoc = glGetUniformLocation( mShaderID, "transform" );
        glGenVertexArrays( 1, &mVAO );
        glGenBuffers( 1, &mVBO );

        const size_t FLOATS_PER_VERTEX = 3;

        glBindVertexArray( mVAO );
            glBindBuffer( GL_ARRAY_BUFFER, mVBO );
            glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(GLfloat), mVertices.data(), GL_DYNAMIC_DRAW );

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

    /* dt = delta frame time in seconds */
    void Update( const float dt )
    {
        /* Increase animation time */
        mAnimTime += dt;
        /* If past total animation time, reset back to beginning */
        if ( mAnimTime >= mTotalAnimTime ) {
            mAnimTime -= mTotalAnimTime;
            mCurrentFrame = 0;
            mNextFrame = 1;
        }

        /* See if we're in between the next pair of keyframes */
        if ( mAnimTime >= mKeyFrames[mNextFrame].time ) {
            /* If so, update which frames we're interpolating between */
            mCurrentFrame = mNextFrame;
            mNextFrame = ( mNextFrame + 1 ) % mKeyFrames.size();
        }

        /* Get the current and next keyframes */
        KeyFrame& curFrame = mKeyFrames[ mCurrentFrame ];
        KeyFrame& nextFrame = mKeyFrames[ mNextFrame ];

        /* Figure out time percentage between current and next frame */
        const float t = ( mAnimTime - curFrame.time ) / 
            ( nextFrame.time - curFrame.time );

        /* Interpolate rotation of current and next frame */
        glm::quat& q1 = curFrame.rotation;
        glm::quat& q2 = nextFrame.rotation;
        glm::quat interpRot = glm::mix( q1, q2, t );
        glm::mat4 rotMat = glm::mat4_cast( interpRot );

        /* Apply the rotation to the default vertices */
        if ( mFrameVertices.size() != mVertices.size() ) {
            mFrameVertices.resize( mVertices.size() );
        }
        for ( size_t i = 0; i < mVertices.size(); i += 3 )
        {
            // TODO - this is really inefficient...
            glm::vec4 inVec( mVertices[i+0], mVertices[i+1], mVertices[i+2], 1.0f );
            glm::vec4 outVec = rotMat * inVec;
            float* outPtr = glm::value_ptr( outVec );
            mFrameVertices[i+0] = outPtr[0];
            mFrameVertices[i+1] = outPtr[1];
            mFrameVertices[i+2] = outPtr[2];
        }
        /* Update the OpenGL buffer */
        glBindBuffer( GL_ARRAY_BUFFER, mVBO );
        glBufferSubData( GL_ARRAY_BUFFER, 0, mFrameVertices.size()*sizeof(GLfloat), mFrameVertices.data() );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    void SetPosition( glm::vec3 pos ) {
        mPosition = pos;
        mTransform = glm::translate( glm::mat4(1.0f), mPosition );
    }

    void Draw( glm::mat4& perspMat )
    {
        // set the transformation matrix value
        mTransform =
            perspMat *
            glm::translate( glm::mat4(1.0f), mPosition );
        glUniformMatrix4fv( mTransformLoc, 1, GL_FALSE, glm::value_ptr(mTransform) );

        //glBindTexture( GL_TEXTURE_2D, mTextureID );
        glBindVertexArray( mVAO );
        const size_t FLOATS_PER_VERTEX = 3;
        glDrawArrays( GL_TRIANGLES, 0, mVertices.size() / FLOATS_PER_VERTEX );
    }

private:
    cgltf_data* mData;
    glm::vec3 mPosition;
    glm::mat4 mTransform;

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

    struct KeyFrame
    {
        float time;
        glm::quat rotation;

        void print()
        {
            float* quatPtr = glm::value_ptr( rotation );
            printf( "Time: %f\n"
                    "Rotation: %f, %f, %f, %f\n",
                 time,
                 quatPtr[0],
                 quatPtr[1],
                 quatPtr[2],
                 quatPtr[3] );
        }
    };
    std::vector<KeyFrame> mKeyFrames;

    void handleAttribData( cgltf_accessor* data )
    {
        printf( "Attrib accessor data name: %s\n", data->name );
        printf( "accessor component type: " );
        switch ( data->component_type )
        {
        case cgltf_component_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_component_type_r_8: printf( "8 Byte\n" ); break;
        case cgltf_component_type_r_8u: printf( "8u unsigned byte\n" ); break;
        case cgltf_component_type_r_16: printf( "16 short\n" ); break;
        case cgltf_component_type_r_16u: printf( "16u unsigned short\n" ); break;
        case cgltf_component_type_r_32u: printf( "32u unsigned int\n" ); break;
        case cgltf_component_type_r_32f: printf( "32f float\n" ); break;
        default:
            break;
        }
        printf( "accessor normalized: %s\n", data->normalized ? "true" : "false" );
        printf( "accessor type: " );
        switch ( data->type )
        {
        case cgltf_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_type_scalar: printf( "scalar\n" ); break;
        case cgltf_type_vec2: printf( "vec2\n" ); break;
        case cgltf_type_vec3: printf( "vec3\n" ); break;
        case cgltf_type_vec4: printf( "vec4\n" ); break;
        case cgltf_type_mat2: printf( "mat2\n" ); break;
        case cgltf_type_mat3: printf( "mat3\n" ); break;
        case cgltf_type_mat4: printf( "mat4\n" ); break;
        default:
            break;
        }
        printf( "Accessor offset: %lu\n", data->offset );
        printf( "Accessor count: %lu\n", data->count );
        printf( "Accessor stride: %lu\n", data->stride );

        if ( data->buffer_view ) {
            printf( "Accessor buffer view not null\n" );
            cgltf_buffer_view* buff = data->buffer_view;
            printf( "Buffer view name: %s\n", buff->name );
            if ( buff->data ) {
                printf( "Buffer view data not null\n" );
            }
            if ( buff->buffer ) {
                printf( "Buffer view buffer not null\n" );
                cgltf_buffer* viewbuf = buff->buffer;
                printf( "view buf name: %s\n", viewbuf->name );
                printf( "view buf size: %lu\n", viewbuf->size );
                if ( viewbuf->data ) {
                    printf( "viewbuf data not null\n" );
                    if ( data->component_type == cgltf_component_type_r_32f &&
                         data->type == cgltf_type_vec3 ) {
                        printf( "  Reading as vec3 float32\n" );
                        size_t byteIndex = buff->offset;
                        unsigned char* byteBuff = (unsigned char*)viewbuf->data;
                        size_t stride = buff->stride == 0 ? data->stride : buff->stride;
                        if ( mVertices.size() != data->count*3 ) {
                            mVertices.resize( data->count*3 );
                        }
                        size_t counter = 0;
                        while ( byteIndex < buff->size && counter < data->count )
                        {
                            float* floatPtr = (float*)&byteBuff[byteIndex];
                            printf( "    %f, %f, %f\n", floatPtr[0], floatPtr[1], floatPtr[2] );
                            byteIndex += stride;
                            mVertices[counter*3 + 0] = floatPtr[0];
                            mVertices[counter*3 + 1] = floatPtr[1];
                            mVertices[counter*3 + 2] = floatPtr[2];
                            ++counter;
                        }
                    }
                    else {
                        printf( "  Error - unhandled component type\n" );
                    }
                }

            }
            printf( "Buffer view offset: %lu\n", buff->offset );
            printf( "Buffer view size: %lu\n", buff->size );
            printf( "Buffer view stride: %lu\n", buff->stride );
        }
    }

    void handleIndices( cgltf_accessor* data )
    {
        printf( "Indices accessor data name: %s\n", data->name );
        printf( "accessor component type: " );
        switch ( data->component_type )
        {
        case cgltf_component_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_component_type_r_8: printf( "8 Byte\n" ); break;
        case cgltf_component_type_r_8u: printf( "8u unsigned byte\n" ); break;
        case cgltf_component_type_r_16: printf( "16 short\n" ); break;
        case cgltf_component_type_r_16u: printf( "16u unsigned short\n" ); break;
        case cgltf_component_type_r_32u: printf( "32u unsigned int\n" ); break;
        case cgltf_component_type_r_32f: printf( "32f float\n" ); break;
        default:
            break;
        }
        printf( "accessor normalized: %s\n", data->normalized ? "true" : "false" );
        printf( "accessor type: " );
        switch ( data->type )
        {
        case cgltf_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_type_scalar: printf( "scalar\n" ); break;
        case cgltf_type_vec2: printf( "vec2\n" ); break;
        case cgltf_type_vec3: printf( "vec3\n" ); break;
        case cgltf_type_vec4: printf( "vec4\n" ); break;
        case cgltf_type_mat2: printf( "mat2\n" ); break;
        case cgltf_type_mat3: printf( "mat3\n" ); break;
        case cgltf_type_mat4: printf( "mat4\n" ); break;
        default:
            break;
        }
        printf( "Accessor offset: %lu\n", data->offset );
        printf( "Accessor count: %lu\n", data->count );
        printf( "Accessor stride: %lu\n", data->stride );

        if ( data->buffer_view ) {
            printf( "Accessor buffer view not null\n" );
            cgltf_buffer_view* buff = data->buffer_view;
            printf( "Buffer view name: %s\n", buff->name );
            if ( buff->data ) {
                printf( "Buffer view data not null\n" );
            }
            if ( buff->buffer ) {
                printf( "Buffer view buffer not null\n" );
                cgltf_buffer* viewbuf = buff->buffer;
                printf( "view buf name: %s\n", viewbuf->name );
                printf( "view buf size: %lu\n", viewbuf->size );
                if ( viewbuf->data ) {
                    printf( "viewbuf data not null\n" );
                    if ( data->component_type == cgltf_component_type_r_16u &&
                         data->type == cgltf_type_scalar ) {
                        printf( "  Reading as scalar unsigned short\n" );
                        size_t byteIndex = buff->offset;
                        unsigned char* byteBuff = (unsigned char*)viewbuf->data;
                        size_t stride = buff->stride == 0 ? data->stride : buff->stride;
                        size_t counter = 0;
                        if ( mIndices.size() != data->count ) {
                            mIndices.resize( data->count );
                        }
                        while ( byteIndex < buff->size && counter < data->count )
                        {
                            unsigned short* shortPtr = (unsigned short*)&byteBuff[byteIndex];
                            printf( "%u, ", shortPtr[0] );
                            byteIndex += stride;
                            mIndices[counter] = shortPtr[0];
                            ++counter;
                        }
                        printf( "\n" );
                    }
                    else {
                        printf( "  Error - unhandled component type\n" );
                    }
                }

            }
            printf( "Buffer view offset: %lu\n", buff->offset );
            printf( "Buffer view size: %lu\n", buff->size );
            printf( "Buffer view stride: %lu\n", buff->stride );
        }
    }

    void handleAnimation( cgltf_animation* anim )
    {
        printf( "animation name: %s\n", anim->name );

        printf( "animation channels count: %lu\n", anim->channels_count );
        for ( size_t i = 0; i < anim->channels_count; ++i )
        {
            cgltf_animation_channel* channel = &anim->channels[i];
            printf( "channel sampler: 0x%p\n", channel->sampler );
            cgltf_animation_sampler* sampler = channel->sampler;
            printf( "sampler input: 0x%p\n", sampler->input );
            handleAnimInputAccessor( sampler->input );
            printf( "sampler output: 0x%p\n", sampler->output );
            handleAnimOutputAccessor( sampler->output );
            printf( "sampler interpolation: " );
            switch ( sampler->interpolation )
            {
            case cgltf_interpolation_type_linear: printf( "linear\n" ); break;
            case cgltf_interpolation_type_step: printf( "step\n" ); break;
            case cgltf_interpolation_type_cubic_spline: printf( "cubic spline\n" ); break;
            default:
                break;
            }
            printf( "channel node: 0x%p\n", channel->target_node );
            handleAnimTargetNode( channel->target_node );
            printf( "channel path type: " );
            switch ( channel->target_path )
            {
            case cgltf_animation_path_type_invalid: printf("invalid\n"); break;
            case cgltf_animation_path_type_translation: printf("translation\n"); break;
            case cgltf_animation_path_type_rotation: printf("rotation\n"); break;
            case cgltf_animation_path_type_scale: printf("scale\n"); break;
            case cgltf_animation_path_type_weights: printf("weights\n"); break;
            default:
                break;
            }
        }
    }

    // should be time of keyframe input
    void handleAnimInputAccessor( cgltf_accessor* input )
    {
        printf( "Anim Attrib accessor data name: %s\n", input->name );
        printf( "accessor component type: " );
        switch ( input->component_type )
        {
        case cgltf_component_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_component_type_r_8: printf( "8 Byte\n" ); break;
        case cgltf_component_type_r_8u: printf( "8u unsigned byte\n" ); break;
        case cgltf_component_type_r_16: printf( "16 short\n" ); break;
        case cgltf_component_type_r_16u: printf( "16u unsigned short\n" ); break;
        case cgltf_component_type_r_32u: printf( "32u unsigned int\n" ); break;
        case cgltf_component_type_r_32f: printf( "32f float\n" ); break;
        default:
            break;
        }
        printf( "accessor normalized: %s\n", input->normalized ? "true" : "false" );
        printf( "accessor type: " );
        switch ( input->type )
        {
        case cgltf_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_type_scalar: printf( "scalar\n" ); break;
        case cgltf_type_vec2: printf( "vec2\n" ); break;
        case cgltf_type_vec3: printf( "vec3\n" ); break;
        case cgltf_type_vec4: printf( "vec4\n" ); break;
        case cgltf_type_mat2: printf( "mat2\n" ); break;
        case cgltf_type_mat3: printf( "mat3\n" ); break;
        case cgltf_type_mat4: printf( "mat4\n" ); break;
        default:
            break;
        }
        printf( "Accessor offset: %lu\n", input->offset );
        printf( "Accessor count: %lu\n", input->count );
        printf( "Accessor stride: %lu\n", input->stride );

        if ( input->buffer_view ) {
            printf( "Accessor buffer view not null\n" );
            cgltf_buffer_view* buff = input->buffer_view;
            printf( "Buffer view name: %s\n", buff->name );
            if ( buff->data ) {
                printf( "Buffer view data not null\n" );
            }
            if ( buff->buffer ) {
                printf( "Buffer view buffer not null\n" );
                cgltf_buffer* viewbuf = buff->buffer;
                printf( "view buf name: %s\n", viewbuf->name );
                printf( "view buf size: %lu\n", viewbuf->size );
                if ( viewbuf->data ) {
                    printf( "viewbuf data not null\n" );
                    if ( input->component_type == cgltf_component_type_r_32f &&
                         input->type == cgltf_type_scalar ) {
                        printf( "  Reading as scalar float32\n" );
                        size_t byteIndex = buff->offset;
                        unsigned char* byteBuff = (unsigned char*)viewbuf->data;
                        size_t stride = buff->stride == 0 ? input->stride : buff->stride;
                        size_t counter = 0;
                        if ( mKeyFrames.size() != input->count ) {
                            mKeyFrames.resize( input->count );
                        }
                        while ( byteIndex < buff->size && counter < input->count )
                        {
                            float* floatPtr = (float*)&byteBuff[byteIndex];
                            printf( "%f\n", floatPtr[0] );
                            byteIndex += stride;
                            mKeyFrames[counter].time = floatPtr[0];
                            ++counter;
                        }
                    }
                    else {
                        printf( "  Error - unhandled component type\n" );
                    }
                }

            }
            printf( "Buffer view offset: %lu\n", buff->offset );
            printf( "Buffer view size: %lu\n", buff->size );
            printf( "Buffer view stride: %lu\n", buff->stride );
        }
    }

    // should be quaternions of rotation output
    void handleAnimOutputAccessor( cgltf_accessor* output )
    {
        printf( "Anim Attrib accessor data name: %s\n", output->name );
        printf( "accessor component type: " );
        switch ( output->component_type )
        {
        case cgltf_component_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_component_type_r_8: printf( "8 Byte\n" ); break;
        case cgltf_component_type_r_8u: printf( "8u unsigned byte\n" ); break;
        case cgltf_component_type_r_16: printf( "16 short\n" ); break;
        case cgltf_component_type_r_16u: printf( "16u unsigned short\n" ); break;
        case cgltf_component_type_r_32u: printf( "32u unsigned int\n" ); break;
        case cgltf_component_type_r_32f: printf( "32f float\n" ); break;
        default:
            break;
        }
        printf( "accessor normalized: %s\n", output->normalized ? "true" : "false" );
        printf( "accessor type: " );
        switch ( output->type )
        {
        case cgltf_type_invalid: printf( "Invalid\n" ); break;
        case cgltf_type_scalar: printf( "scalar\n" ); break;
        case cgltf_type_vec2: printf( "vec2\n" ); break;
        case cgltf_type_vec3: printf( "vec3\n" ); break;
        case cgltf_type_vec4: printf( "vec4\n" ); break;
        case cgltf_type_mat2: printf( "mat2\n" ); break;
        case cgltf_type_mat3: printf( "mat3\n" ); break;
        case cgltf_type_mat4: printf( "mat4\n" ); break;
        default:
            break;
        }
        printf( "Accessor offset: %lu\n", output->offset );
        printf( "Accessor count: %lu\n", output->count );
        printf( "Accessor stride: %lu\n", output->stride );

        if ( output->buffer_view ) {
            printf( "Accessor buffer view not null\n" );
            cgltf_buffer_view* buff = output->buffer_view;
            printf( "Buffer view name: %s\n", buff->name );
            if ( buff->data ) {
                printf( "Buffer view data not null\n" );
            }
            if ( buff->buffer ) {
                printf( "Buffer view buffer not null\n" );
                cgltf_buffer* viewbuf = buff->buffer;
                printf( "view buf name: %s\n", viewbuf->name );
                printf( "view buf size: %lu\n", viewbuf->size );
                if ( viewbuf->data ) {
                    printf( "viewbuf data not null\n" );
                    if ( output->component_type == cgltf_component_type_r_32f &&
                         output->type == cgltf_type_vec4 ) {
                        printf( "  Reading as vec4 float32\n" );
                        size_t byteIndex = output->offset > 0 ? output->offset : buff->offset;
                        unsigned char* byteBuff = (unsigned char*)viewbuf->data;
                        size_t stride = buff->stride == 0 ? output->stride : buff->stride;
                        size_t counter = 0;
                        if ( mKeyFrames.size() != output->count )
                        {
                            mKeyFrames.resize( output->count );
                        }
                        while ( byteIndex < buff->size && counter < output->count )
                        {
                            float* floatPtr = (float*)&byteBuff[byteIndex];
                            printf( "%f, %f, %f, %f\n",
                                    floatPtr[0],
                                    floatPtr[1],
                                    floatPtr[2],
                                    floatPtr[3] );
                            byteIndex += stride;

                            mKeyFrames[counter].rotation = glm::quat(
                                floatPtr[3], // w
                                floatPtr[0], // x
                                floatPtr[1], // y
                                floatPtr[2]  // z
                            );

                            ++counter;
                        }
                    }
                    else {
                        printf( "  Error - unhandled component type\n" );
                    }
                }

            }
            printf( "Buffer view offset: %lu\n", buff->offset );
            printf( "Buffer view size: %lu\n", buff->size );
            printf( "Buffer view stride: %lu\n", buff->stride );
        }
    }

    void handleAnimTargetNode( cgltf_node* node )
    {
        cgltf_mesh* mesh = node->mesh;
        printf( "Mesh name: %s\n", mesh->name );
        printf( "Mesh target names: " );
        for ( size_t k=0; k<mesh->target_names_count; ++k ) {
            printf( "%s, ", mesh->target_names[k] );
        }
        printf( "\n" );
        for ( size_t k=0; k<mesh->primitives_count; ++k ) {
            cgltf_primitive& prim = mesh->primitives[k];
            cgltf_primitive_type type = prim.type;
            printf( "Prim Type: " );
            switch ( type )
            {
            case cgltf_primitive_type_points: printf( "points\n" ); break;
            case cgltf_primitive_type_lines: printf( "lines\n" ); break;
            case cgltf_primitive_type_line_loop: printf( "loop\n" ); break;
            case cgltf_primitive_type_line_strip: printf( "strip\n" ); break;
            case cgltf_primitive_type_triangles: printf( "triangles\n" ); break;
            case cgltf_primitive_type_triangle_strip: printf( "triangle strip\n" ); break;
            case cgltf_primitive_type_triangle_fan: printf( "fan\n" ); break;
            default:
                break;
            }

            if ( prim.indices ) {
                printf( "Indices not null\n" );
                handleIndices( prim.indices );
            }
            if ( prim.material ) {
                printf( "material not null\n" );
            }
            if ( prim.attributes_count > 0 ) {
                printf( "Attributes count > 0\n" );
                for ( size_t l=0; l<prim.attributes_count; ++l ) {
                    cgltf_attribute& attrib = prim.attributes[l];
                    printf( "Attrib Name: %s\n", attrib.name );
                    printf( "Attrib type: " );
                    switch ( attrib.type )
                    {
                    case cgltf_attribute_type_invalid: printf( "Invalid\n" ); break;
                    case cgltf_attribute_type_position: printf( "Position\n" ); break;
                    case cgltf_attribute_type_normal: printf( "normal\n" ); break;
                    case cgltf_attribute_type_tangent: printf( "tangent\n" ); break;
                    case cgltf_attribute_type_texcoord: printf( "texcoord\n" ); break;
                    case cgltf_attribute_type_color: printf( "color\n" ); break;
                    case cgltf_attribute_type_joints: printf( "joints\n" ); break;
                    case cgltf_attribute_type_weights: printf( "weights\n" ); break;
                    }
                    printf( "Attrib index: %d\n", attrib.index );
                    if ( attrib.data ) {
                        printf( "Attrib data not null\n" );
                        cgltf_accessor* data = attrib.data;
                        handleAttribData( data );
                    }
                }
            }
        }
    }
};

#endif // GLTF_MESH_H_INCLUDED

