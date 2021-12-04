#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "cgltf.h"

#ifdef WIN32
#undef main
#endif

glm::mat4 perspMat = glm::perspective(
    45.0f * 3.1415926535f / 180.0f, // FOV
    640.0f / 480.0f, // aspect
    0.1f, // near
    1000.0f // far
);

class GLTFMesh
{
public:
    GLTFMesh() :
        mData( nullptr ),
        mPosition( 0.0f, 0.0f, 0.0f, 1.0f ),
        mRotation( 0.0f )
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

    bool Load(const char* filename)
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

        printf( "GLTF num scenes: %lu\n", mData->scenes_count );
        for ( size_t i=0; i<mData->scenes_count; ++i)
        {
            cgltf_scene& scene = mData->scenes[i];
            printf( "Scene %lu: %s\n", i, scene.name );
            printf( "Num nodes: %lu\n", scene.nodes_count );
            for ( size_t j=0; j<scene.nodes_count; ++j )
            {
                glm::mat4 parentMat( 1.0f );
                cgltf_node* node = scene.nodes[j];
                processNode( j, parentMat, node );
            }
        }

        printf( "Node positions:\n" );
        for ( glm::vec4& pos : mNodePositions )
        {
            float* f = glm::value_ptr( pos );
            printf( "%f, %f, %f, %f\n", f[0], f[1], f[2], f[3] );
        }

        return true;
    }

    void CreateBuffers( GLuint shaderID )
    {
        mShaderID = shaderID;
        mTransformLoc = glGetUniformLocation( mShaderID, "transform" );

        glGenVertexArrays( 1, &mVAO );
        glGenBuffers( 1, &mVBO );

        glBindVertexArray( mVAO );
            glBindBuffer( GL_ARRAY_BUFFER, mVBO );
            glBufferData(
                GL_ARRAY_BUFFER,
                mNodePositions.size() * sizeof( glm::vec4 ),
                (void*)mNodePositions.data(),
                GL_STATIC_DRAW
            );
            const std::size_t FLOATS_PER_VERT = 4;
            glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, FLOATS_PER_VERT*sizeof(GLfloat), (void*)0 );
            glEnableVertexAttribArray( 0 ); // location=0
            
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    void Draw()
    {
        mRotation += 0.1f;
        glm::mat4 transform =
            perspMat *
            glm::translate( glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -10.0f) ) *
            glm::rotate( glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 1.0f, 0.0f) );
        glUniformMatrix4fv( mTransformLoc, 1, GL_FALSE, glm::value_ptr(transform) );

        glBindVertexArray( mVAO );
        glDisable( GL_POINT_SMOOTH );
        glPointSize( 4.0f );
        glDrawArrays( GL_POINTS, 0, mNodePositions.size() );
    }

private:
    cgltf_data* mData;

    glm::vec4 mPosition;
    std::vector<glm::vec4> mNodePositions;

    GLuint mShaderID;
    GLuint mTransformLoc;
    GLuint mVAO, mVBO;

    GLfloat mRotation;

    inline void processNode( size_t parentIndex, glm::mat4 parentWorldMat, cgltf_node* node )
    {
        printNode( parentIndex, node );
        
        glm::quat nodeRotation = glm::angleAxis( 0.0f, glm::vec3(1.0f, 0.0, 0.0f) );
        glm::vec3 nodeTranslation(0.0f, 0.0f, 0.0f);
        glm::vec3 nodeScale(1.0f, 1.0f, 1.0f);

        if ( node->has_translation ) {
            printf( "Has Translation: (%f, %f, %f)\n",
                node->translation[0],
                node->translation[1],
                node->translation[2]
            );
            nodeTranslation = glm::vec3(
                node->translation[0],
                node->translation[1],
                node->translation[2]
            );
        }
        if ( node->has_rotation) {
            printf( "Has Rotation: (%f, %f, %f, %f)\n",
                node->rotation[0],
                node->rotation[1],
                node->rotation[2],
                node->rotation[3]
            );
            nodeRotation = glm::quat(
                node->rotation[0],
                node->rotation[1],
                node->rotation[2],
                node->rotation[3]
            );
        }
        if ( node->has_scale ) {
            printf( "Has Scale: (%f, %f, %f)\n",
                node->scale[0],
                node->scale[1],
                node->scale[2]
            );
            nodeScale = glm::vec3(
                node->scale[0],
                node->scale[1],
                node->scale[2]
            );
        }
        if ( node->has_matrix ) {
            printf( "Has Matrix:\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n",
                node->matrix[0],
                node->matrix[1],
                node->matrix[2],
                node->matrix[3],
                node->matrix[4],
                node->matrix[5],
                node->matrix[6],
                node->matrix[7],
                node->matrix[8],
                node->matrix[9],
                node->matrix[10],
                node->matrix[11],
                node->matrix[12],
                node->matrix[13],
                node->matrix[14],
                node->matrix[15]
            );
        }
        glm::mat4 nodeLocalMat =
            glm::translate( glm::mat4(1.0f), nodeTranslation ) * 
            glm::mat4_cast( nodeRotation ) *
            glm::scale( glm::mat4(1.0f), nodeScale );
        {
            float* matPtr = glm::value_ptr( nodeLocalMat );
            printf( "Local Matrix:\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n",
                matPtr[0],
                matPtr[1],
                matPtr[2],
                matPtr[3],
                matPtr[4],
                matPtr[5],
                matPtr[6],
                matPtr[7],
                matPtr[8],
                matPtr[9],
                matPtr[10],
                matPtr[11],
                matPtr[12],
                matPtr[13],
                matPtr[14],
                matPtr[15]
            );
        }

        glm::mat4 nodeWorldMat = parentWorldMat * nodeLocalMat;
        {
            float* matPtr = glm::value_ptr( nodeWorldMat );
            printf( "World Matrix:\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n",
                matPtr[0],
                matPtr[1],
                matPtr[2],
                matPtr[3],
                matPtr[4],
                matPtr[5],
                matPtr[6],
                matPtr[7],
                matPtr[8],
                matPtr[9],
                matPtr[10],
                matPtr[11],
                matPtr[12],
                matPtr[13],
                matPtr[14],
                matPtr[15]
            );
        }

        mNodePositions.push_back( nodeWorldMat * mPosition );

        for ( size_t i=0; i<node->children_count; ++i ) {
            cgltf_node* childNode = node->children[i];
            processNode( parentIndex+1, nodeWorldMat, childNode );
        }
    }

    inline void printNode( const size_t nodeNum, cgltf_node* node )
    {
        printf( "Node %lu: %s\n", nodeNum, node->name );
        printf( "Parent: 0x%p\n", (void*)node->parent );
        printf( "Num children: %lu\n", node->children_count );

        glm::quat nodeRotation;
        glm::vec3 nodeTranslation(0.0f, 0.0f, 0.0f);
        glm::vec3 nodeScale(1.0f, 1.0f, 1.0f);

        if ( node->has_translation ) {
            printf( "Has Translation: (%f, %f, %f)\n",
                node->translation[0],
                node->translation[1],
                node->translation[2]
            );
            nodeTranslation = glm::vec3(
                node->translation[0],
                node->translation[1],
                node->translation[2]
            );
        }
        if ( node->has_rotation) {
            printf( "Has Rotation: (%f, %f, %f, %f)\n",
                node->rotation[0],
                node->rotation[1],
                node->rotation[2],
                node->rotation[3]
            );
            nodeRotation = glm::quat(
                node->rotation[0],
                node->rotation[1],
                node->rotation[2],
                node->rotation[3]
            );
        }
        if ( node->has_scale ) {
            printf( "Has Scale: (%f, %f, %f)\n",
                node->scale[0],
                node->scale[1],
                node->scale[2]
            );
            nodeScale = glm::vec3(
                node->scale[0],
                node->scale[1],
                node->scale[2]
            );
        }
        if ( node->has_matrix ) {
            printf( "Has Matrix:\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n",
                node->matrix[0],
                node->matrix[1],
                node->matrix[2],
                node->matrix[3],
                node->matrix[4],
                node->matrix[5],
                node->matrix[6],
                node->matrix[7],
                node->matrix[8],
                node->matrix[9],
                node->matrix[10],
                node->matrix[11],
                node->matrix[12],
                node->matrix[13],
                node->matrix[14],
                node->matrix[15]
            );
        }
        glm::mat4 nodeLocalMat =
            glm::translate( glm::mat4(1.0f), nodeTranslation ) * 
            glm::mat4_cast( nodeRotation ) *
            glm::scale( glm::mat4(1.0f), nodeScale );
        {
            float* matPtr = glm::value_ptr( nodeLocalMat );
            printf( "Local Matrix:\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n"
                    "%f, %f, %f, %f\n",
                matPtr[0],
                matPtr[1],
                matPtr[2],
                matPtr[3],
                matPtr[4],
                matPtr[5],
                matPtr[6],
                matPtr[7],
                matPtr[8],
                matPtr[9],
                matPtr[10],
                matPtr[11],
                matPtr[12],
                matPtr[13],
                matPtr[14],
                matPtr[15]
            );
        }

        cgltf_skin* skin = node->skin;
        if ( skin ) {
            printf( "Skin name: %s\n", skin->name );
            printf( "Skin joint names: " );
            for ( size_t i=0; i<skin->joints_count; ++i ) {
                printf( "%s, ", skin->joints[i]->name );
            }
            if ( skin->skeleton ) {
                printf( "Skin skeleton name: %s\n", skin->skeleton->name );
            }
        }
        
        cgltf_mesh* mesh = node->mesh;
        if ( mesh ) {
            printf( "Mesh name: %s\n", mesh->name );
            printf( "Mesh weights: " );
            for ( size_t i=0; i<mesh->weights_count; ++i ) {
                printf( "%f, ", mesh->weights[i] );
            }
            printf( "\n" );
            printf( "Target names: " );
            for ( size_t i=0; i<mesh->target_names_count; ++i ) {
                printf( "%s, ", mesh->target_names[i] );
            }
            printf( "\n" );
        }

        if ( node->weights_count > 0 ) {
            printf( "Has weights: " );
            for ( size_t k = 0; k < node->weights_count; ++k ) {
                printf( "%f, ", node->weights[k] );
            }
            printf( "\n" );
        }
    }

};

int main()
{

    Renderer render( "SDL2 Window", 640, 480 );


    //Shader shader( "shaders/VertexShader.glsl", "shaders/FragmentShader.glsl" );
    Shader skelTestShader( "shaders/SkelVertShader.glsl", "shaders/SkelFragShader.glsl" );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
#if 0
    std::vector<GLfloat> vertices = {
        // vertices            // color             // texture coords
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
         0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.5f, 1.0f  // top
    };
#endif
    
        // vertices
    /*std::vector<GLfloat> skelVertices = {
        0.000000, 0.000000, 0.000000,   
        0.000000, 0.000000, 0.000000,   
        0.021929, 2.766668, -0.037960,  
        0.026433, 2.485877, -0.060244,  
        0.034633, 2.810832, -0.011704,  
        0.022455, 2.435777, -0.028215,  
        0.043730, 2.853359, 0.032905,   
        -0.018662, 2.585028, -0.015179, 
        0.163367, 3.312670, 0.006888,   
        0.202182, 2.801513, 0.020483,   
        -0.106174, 2.927312, 0.071622,  
        0.010585, 2.820728, -0.621027,  
        -0.128309, 2.770478, -0.075166, 
        -0.044961, 2.713219, -0.203717, 
        -0.103212, 2.639395, -0.112737, 
        -0.007094, 2.675986, -0.169323, 
        -0.074343, 2.637132, -0.117752, 
        -0.098709, 2.824789, -0.325035, 
        -0.091279, 2.779897, -0.145784, 
        -0.121464, 2.859628, -0.269606, 
        -0.096617, 2.813317, -0.214438, 
        -0.119203, 2.815925, 0.033750,  
        -0.428981, 2.948354, 0.038596,  
        -0.080966, 2.651614, 0.581712,  
        -0.423020, 2.726519, 0.137675,  
        -0.440788, 2.784585, 0.293264,  
        -0.516727, 2.710047, 0.212457,  
        -0.420142, 2.772408, 0.233335,  
        -0.490471, 2.716568, 0.206676,  
        -0.277997, 2.745053, 0.389716,  
        -0.332150, 2.736941, 0.247858,  
        -0.323126, 2.775238, 0.391867,  
        -0.310089, 2.747768, 0.317319,  
        -0.234309, 2.914197, 0.020886,  
        -0.690462, 1.785556, -0.340937, 
        0.239440, 2.582991, -0.350524,  
        0.378242, 2.215542, -0.488350,  
        0.261880, 2.310126, -0.426306,  
        0.275310, 2.923631, 0.005473,   
        0.233999, 1.702392, -0.338793,  
        0.665536, 2.815365, -0.066349,  
        0.992879, 2.563720, -0.005025,  
        0.835427, 2.589293, -0.046202
    };*/


    //Mesh mesh( vertices, shader.GetProgID() );
    //mesh.LoadTexture( "data/wall.jpg" );

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //mesh.Rotate( glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f) );

    GLTFMesh gltf;
    if ( !gltf.Load("data/Woman.gltf") ) {
        return 1;
    }
    skelTestShader.Use();
    gltf.CreateBuffers( skelTestShader.GetProgID() );


    // render loop
    // -----------
    while ( !render.ShouldClose() )
    {
        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();

        // draw our first triangle
        //shader.Use();
        //mesh.Draw();
        skelTestShader.Use();
        gltf.Draw();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        render.Update();
    }

    return 0;
}

