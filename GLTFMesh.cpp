#include "GLTFMesh.h"

GLTFMesh::GLTFMesh() :
    mData( nullptr ),
    mPosition( 0.0f, 0.0f, 0.0f ),
    mRotation( 0.0f ),
    mCurrentFrame( 0 ),
    mNextFrame( 1 ),
    mAnimTime( 0.0f ),
    mCurAnim( nullptr )
{
}

GLTFMesh::~GLTFMesh()
{
    if ( mData ) {
        cgltf_free( mData );
        mData = nullptr;
    }
    //glDeleteVertexArrays( 1, &mVAO );
    //glDeleteBuffers( 1, &mVBO );
}

bool GLTFMesh::Load( const char* filename )
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
    
    /* Must happenn after load nodes so mCgltfNodeToIntMap is filled */
    printf( "Animations count: %lu\n", mData->animations_count );
    mAnimations.resize( mData->animations_count );
    for ( size_t i = 0; i < mData->animations_count; ++i )
    {
        loadAnimation( mData->animations[i], mAnimations[i] );
        mAnimations[i].calcStartEndTime();
        //mAnimations[i].print();
    }
    if ( mAnimations.size() > 0 ) {
        mCurAnim = &mAnimations[0];
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
            loadSkelVertNode( rootNode.children[i],
                transform,
                resIndex );
        }
    }

    return true;
}

void GLTFMesh::Update( const float dt )
{
    if ( !mCurAnim ) {
        printf( "mCurAnim is null\n" );
        return;
    }
    
    /* Update animation time */
    const float speedScale = 1.0f;
    mAnimTime += dt * speedScale;
    if ( mAnimTime > mCurAnim->endTime ) {
        mAnimTime = mCurAnim->startTime + ( mAnimTime - mCurAnim->endTime );
    }        

    glm::vec4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
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
        loadSkelVertNode( rootNode.children[i],
            transform,
            resIndex );
    }
    
    /* Update vertices buffer */
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, mFrameVertices.size()*sizeof(GLfloat), mFrameVertices.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void GLTFMesh::CreateBuffers( GLuint shaderID )
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

void GLTFMesh::loadAnimation( cgltf_animation& anim, Animation& result )
{
    if ( anim.name && strlen( anim.name ) > 0 ) {
        result.name = std::string( anim.name );
    } else {
        result.name = "Unknown";
    }
    
    for ( size_t i = 0; i < anim.channels_count; ++i )
    {
        cgltf_animation_channel& channel = anim.channels[i];
        cgltf_node* targetNode = channel.target_node;
        if ( mCgltfNodeToIntMap.find( targetNode ) == mCgltfNodeToIntMap.end() )
        {
            printf( "Error - failed to find cgltf node to int anim target node\n" );
            return;
        }
        int nodeIndex = mCgltfNodeToIntMap[targetNode];
        int nodeAnimIndex = -1;
        for ( size_t j = 0; j < result.nodeAnims.size(); ++j )
        {
            if ( result.nodeAnims[j].node == nodeIndex ) {
                nodeAnimIndex = j;
                break;
            }
        }
        if ( nodeAnimIndex == -1 ) {
            result.nodeAnims.push_back( NodeAnimation() );
            nodeAnimIndex = result.nodeAnims.size() - 1;
            result.nodeAnims[nodeAnimIndex].node = nodeIndex;
        }
        NodeAnimation& nodeAnim = result.nodeAnims[nodeAnimIndex];
        
        switch ( channel.target_path )
        {
        case cgltf_animation_path_type_translation: loadAnimTranslation( channel, nodeAnim.transKeyFrames ); break;
        case cgltf_animation_path_type_rotation: loadAnimRotation( channel, nodeAnim.rotKeyFrames ); break;
        case cgltf_animation_path_type_scale: loadAnimScale( channel, nodeAnim.scaleKeyFrames ); break;
        case cgltf_animation_path_type_weights: printf( "Unhandled weights target path\n" ); break;
        default:
            break;
        }
    }
}

void GLTFMesh::loadAnimTranslation(
    cgltf_animation_channel& channel,
    std::vector<KeyFrameTrans>& result )
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
        result[i].translation = glm::vec3( translations[i] );
    }
}

void GLTFMesh::loadAnimRotation( cgltf_animation_channel& channel, std::vector<KeyFrameRot>& result )
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
        result[i].rotation = glm::quat( rotations[i] );
    }
}

void GLTFMesh::loadAnimScale( cgltf_animation_channel& channel, std::vector<KeyFrameScale>& result )
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
        result[i].scale = glm::vec3( scales[i] );
    }
}
    
void GLTFMesh::loadKeyFrameTimes( cgltf_animation_channel& channel, std::vector<float>& result )
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
    while ( byteIndex < buffer.size && counter < accessor.count )
    {
        float* floatPtr = (float*)&byteBuff[byteIndex];
        result[counter] = *floatPtr;
        //printf( "loadKeyframeTime: %f\n", result[counter] );
        ++counter;
        byteIndex += stride;
    }
}
    
void GLTFMesh::loadKeyFrameVec3s( cgltf_animation_channel& channel, std::vector<glm::vec3>& result )
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
    while ( byteIndex < buffer.size && counter < accessor.count )
    {
        float* floatPtr = (float*)&byteBuff[byteIndex];
        result[counter] = glm::vec3( floatPtr[0], floatPtr[1], floatPtr[2] );
        //printf( "loadKeyframeVec3s: %f,%f,%f\n", floatPtr[0], floatPtr[1], floatPtr[2] );
        ++counter;
        byteIndex += stride;
    }
}
    
void GLTFMesh::loadKeyFrameQuats( cgltf_animation_channel& channel, std::vector<glm::quat>& result )
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
    while ( byteIndex < buffer.size && counter < accessor.count )
    {
        float* floatPtr = (float*)&byteBuff[byteIndex];
        result[counter] = glm::quat( floatPtr[3], floatPtr[0], floatPtr[1], floatPtr[2] );
        ++counter;
        byteIndex += stride;
    }
}
    
void GLTFMesh::loadNodes( cgltf_node& rootNode, std::vector<Node>& result )
{
    if ( rootNode.parent != nullptr ) {
        printf( "Error - expected root node to have no parent\n" );
        return;
    }
    
    size_t nodeIndex = 0;
    Node& node = result[0];
    if ( rootNode.name != nullptr ) {
        node.name = std::string( rootNode.name );
    } else {
        node.name = "Unknown";
    }
    node.parent = -1;
    loadNodeTransforms( rootNode, node );
    int nodeCounter = 0;
    mCgltfNodeToIntMap[&rootNode] = 0;
    for ( size_t i = 0; i < rootNode.children_count; ++i )
    {
        loadNode( *rootNode.children[i], 0, nodeCounter, result );
    }
}
    
void GLTFMesh::loadNode( cgltf_node& node,
    const int parentIndex,
    int& nodeIndexCounter,
    std::vector<Node>& result )
{
    ++nodeIndexCounter;
    const int thisNodeIndex = nodeIndexCounter;
    result[parentIndex].children.push_back( thisNodeIndex );
    result[thisNodeIndex].parent = parentIndex;
    if ( node.name ) {
        result[thisNodeIndex].name = std::string( node.name );
    }
    loadNodeTransforms( node, result[thisNodeIndex] );
    mCgltfNodeToIntMap[&node] = thisNodeIndex;
    for ( size_t i = 0; i < node.children_count; ++i )
    {
        loadNode( *node.children[i], thisNodeIndex, nodeIndexCounter, result );
    }
}

void GLTFMesh::loadNodeTransforms( cgltf_node& node, Node& result )
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
    
void GLTFMesh::loadSkelVertNode( const int nodeIndex, glm::mat4& parentTransform, size_t& resIndex )
{
    Node& node = mNodes[nodeIndex];
    
    /* Find the node animation for this node */
    NodeAnimation* nodeAnim = nullptr;
    for ( size_t animIndex = 0; animIndex < mCurAnim->nodeAnims.size(); ++animIndex )
    {
        /* Zero is the root node index */
        if ( mCurAnim->nodeAnims[animIndex].node == nodeIndex ) {
            nodeAnim = &mCurAnim->nodeAnims[animIndex];
            break;
        }
    }

    glm::quat interpRot = node.rotation;
    glm::vec3 interpTrans = node.translation;
    glm::vec3 interpScale = node.scale;
    //glm::quat interpRot = glm::angleAxis( 0.0f, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    //glm::vec3 interpTrans = glm::vec3( 0.0f, 0.0f, 0.0f );
    //glm::vec3 interpScale = glm::vec3( 1.0f, 1.0f, 1.0f );
    
    if ( nodeAnim )
    {
    if ( nodeAnim->rotKeyFrames.size() > 0 ) {
        std::vector<KeyFrameRot>& rotFrames = nodeAnim->rotKeyFrames;
        /* Find what our current and next keyframes are based on current time */
        KeyFrameRot* curFrame = nullptr;
        size_t curFrameIndex = 0;
        size_t nextFrameIndex = 0;
        for ( size_t i = 0; i < rotFrames.size(); ++i ) {
            if ( rotFrames[i].time <= mAnimTime &&
                 rotFrames[(i+1) % rotFrames.size()].time > mAnimTime )
            {
                curFrame = &rotFrames[i];
                curFrameIndex = i;
                nextFrameIndex = (i+1) % rotFrames.size();
                break;
            }
        }
        if ( curFrame != nullptr ) {
            KeyFrameRot* nextFrame = &rotFrames[nextFrameIndex];
            const float t = ( mAnimTime - curFrame->time ) /
                ( nextFrame->time - curFrame->time );

            /* Hands on game animation programming, pg 89,
             * Interpolating Quaternions, neighborhood */            
            if ( glm::dot( curFrame->rotation, nextFrame->rotation ) < 0.0f ) {
                glm::quat nextRot = -nextFrame->rotation;
                interpRot = glm::mix( curFrame->rotation, nextRot, t );
            }
            else {
                interpRot = glm::mix( curFrame->rotation, nextFrame->rotation, t );
            }
        }
        else {
            printf( "Failed to find cur rot frame\n" );
            assert( false );
        }
    }
    if ( nodeAnim->transKeyFrames.size() > 0 ) {
        std::vector<KeyFrameTrans>& transFrames = nodeAnim->transKeyFrames;
        /* Find what our current and next keyframes are based on current time */
        KeyFrameTrans* curFrame = nullptr;
        size_t curFrameIndex = 0;
        size_t nextFrameIndex = 0;
        for ( size_t i = 0; i < transFrames.size(); ++i ) {
            if ( transFrames[i].time <= mAnimTime &&
                 transFrames[(i+1) % transFrames.size()].time > mAnimTime )
            {
                curFrame = &transFrames[i];
                curFrameIndex = i;
                nextFrameIndex = (i+1) % transFrames.size();
                break;
            }
        }
        if ( curFrame != nullptr ) {
            KeyFrameTrans* nextFrame = &transFrames[nextFrameIndex];
            const float t = ( mAnimTime - curFrame->time ) /
                ( nextFrame->time - curFrame->time );
            interpTrans = glm::mix( curFrame->translation, nextFrame->translation, t );
        }
        else {
            printf( "Failed to find cur trans frame" );
            assert( false );
        }
    }
    if ( nodeAnim->scaleKeyFrames.size() > 0 ) {
        std::vector<KeyFrameScale>& scaleFrames = nodeAnim->scaleKeyFrames;
        /* Find what our current and next keyframes are based on current time */
        KeyFrameScale* curFrame = nullptr;
        size_t curFrameIndex = 0;
        size_t nextFrameIndex = 0;
        for ( size_t i = 0; i < scaleFrames.size(); ++i ) {
            if ( scaleFrames[i].time <= mAnimTime &&
                 scaleFrames[(i+1) % scaleFrames.size()].time > mAnimTime )
            {
                curFrame = &scaleFrames[i];
                curFrameIndex = i;
                nextFrameIndex = (i+1) % scaleFrames.size();
                break;
            }
        }
        if ( curFrame != nullptr ) {
            KeyFrameScale* nextFrame = &scaleFrames[nextFrameIndex];
            const float t = ( mAnimTime - curFrame->time ) /
                ( nextFrame->time - curFrame->time );
            interpScale = glm::mix( curFrame->scale, nextFrame->scale, t );
        }
        else {
            printf( "Failed to find cur scale frame" );
            assert( false );
        }
    }
    }
    
    glm::mat4 animMat = glm::translate( glm::mat4(1.0f), interpTrans ) *
            glm::mat4_cast( interpRot ) *
            glm::scale( glm::mat4(1.0f), interpScale );
    
    glm::vec4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
    glm::mat4 transform = parentTransform * animMat;
    pos = transform * pos;
    float* fPtr = glm::value_ptr( pos );
    mFrameVertices[resIndex+0] = fPtr[0];
    mFrameVertices[resIndex+1] = fPtr[1];
    mFrameVertices[resIndex+2] = fPtr[2];
    resIndex += 3; // 3 floats per vertex
    
    for ( size_t i = 0; i < node.children.size(); ++i )
    {
        loadSkelVertNode( node.children[i],
            transform,
            resIndex );
    }
}
