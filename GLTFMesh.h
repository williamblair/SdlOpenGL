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
        mPosition( 0.0f, 0.0f, 0.0f, 1.0f )
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
                cgltf_node* node = scene.nodes[j];
                if ( node->mesh ) {
                    cgltf_mesh* mesh = node->mesh;
                    if ( mesh->name ) {
                        printf( "Mesh name: %s\n", mesh->name );
                    }
                    printf( "Mesh target names: " );
                    for ( size_t k=0; k<mesh->target_names_count; ++k ) {
                        printf( "%s, ", mesh->target_names[i] );
                    }
                    printf( "\n" );
                    if ( !mesh->primitives || mesh->primitives_count == 0 ) {
                        continue;
                    }
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
            }
        }

        return true;
    }

private:
    cgltf_data* mData;

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
                        while ( byteIndex < buff->size )
                        {
                            float* floatPtr = (float*)&byteBuff[byteIndex];
                            printf( "    %f, %f, %f\n", floatPtr[0], floatPtr[1], floatPtr[2] );
                            byteIndex += stride;
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
                        while ( byteIndex < buff->size )
                        {
                            unsigned short* shortPtr = (unsigned short*)&byteBuff[byteIndex];
                            printf( "%u, ", shortPtr[0] );
                            byteIndex += stride;
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

    glm::vec4 mPosition;
};

#endif // GLTF_MESH_H_INCLUDED

