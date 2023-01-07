#include "AssimpMesh.h"
#include "Renderer.h"

#include <cassert>

static inline glm::mat4 aiMatToMat4( const aiMatrix4x4& mat )
{
    // a,b,c,d = row; 1,2,3,4 = col
    float vals[16] = {
        mat.a1, mat.b1, mat.c1, mat.d1,
        mat.a2, mat.b2, mat.c2, mat.d2,
        mat.a3, mat.b3, mat.c3, mat.d3,
        mat.a4, mat.b4, mat.c4, mat.d4
    };
    return glm::make_mat4( vals );
}

AssimpMesh::AssimpMesh( const std::string& fileName ) :
    mAnimation(nullptr)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        fileName.c_str(),
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals
    );
    if ( !scene ||
            (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
            !scene->mRootNode ) {
        std::cerr << "AssimpMesh::Load readfile failed: " <<
            importer.GetErrorString() << std::endl;
        exit( EXIT_FAILURE );
    }
    mMeshes.resize( scene->mNumMeshes );
    mSkeletons.resize( scene->mNumMeshes );
    mPalette.resize( scene->mNumMeshes );
    mCurrentPoses.resize( scene->mNumMeshes );
    size_t curMesh = 0;
    if ( !processNode( scene->mRootNode, scene, curMesh )) {
        std::cerr << "AssimpMesh::Load failed to load meshes" << std::endl;
        exit( EXIT_FAILURE );
    }
    
    mAnimations.resize( scene->mNumAnimations );
    mAnimNames.resize( scene->mNumAnimations );
    for ( size_t i=0; i<scene->mNumAnimations; ++i ) {
        mAnimNames[i] = std::string( scene->mAnimations[i]->mName.C_Str() );
        if ( !mAnimations[i].Load( scene->mAnimations[i], scene, &mSkeletons[0] )) {
            std::cerr << "AssimpMesh::Load failed to process animation" << std::endl;
            exit( EXIT_FAILURE );
        }
    }
    if ( mAnimations.size() == 1 && mAnimations[0].GetName().empty() ) {
        mAnimNames[0] = "animation0";
        mAnimations[0].SetName("animation0");
    }
    
    mAnimation = &mAnimations[0];
    mAnimTime = 0.0f;
    mAnimPlayRate = 1.0f;

    std::cout << "Loaded Assimp Mesh " << fileName << std::endl;
    std::cout << "  Num Meshes: " << mMeshes.size() << std::endl;
    std::cout << "  Animations: " << mAnimations.size() << std::endl;
    for (std::string& name : mAnimNames) {
        std::cout << "    " << name << std::endl;
    }
}
AssimpMesh::~AssimpMesh()
{
}

bool AssimpMesh::processNode(
    const aiNode* node,
    const aiScene* scene,
    size_t& curMesh
)
{
    // process all the node's meshes (if any)
    for ( size_t i=0; i<node->mNumMeshes; ++i ) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        assert( curMesh < mMeshes.size() );
        if ( !mMeshes[curMesh].Load(mesh, scene) ) {
            return false;
        }
        assert( curMesh < mSkeletons.size() );
        if ( !mSkeletons[curMesh].Load( mesh, scene )) {
            return false;
        }
        ++curMesh;
    }

    // process the node's children
    for ( size_t i=0; i<node->mNumChildren; ++i ) {
        if ( !processNode( node->mChildren[i], scene, curMesh )) {
            return false;
        }
    }

    return true;
}

void AssimpMesh::Update( const float dt )
{
    if ( !mAnimation ) {
        return;
    }

    mAnimTime += dt * mAnimPlayRate;
    while ( mAnimTime > mAnimation->GetDuration() ) {
        mAnimTime -= mAnimation->GetDuration();
    }

    for ( size_t mshIdx = 0; mshIdx < mMeshes.size(); ++mshIdx )
    {
        Mesh& mMesh = mMeshes[mshIdx];
        Skeleton* mSkeleton = &mSkeletons[mshIdx];
        if ( mSkeleton->GetNumBones() == 0 ) { continue; }

        // Update matrices
        ComputeMatrixPalette( mshIdx );

        // Update vertex buffer
        const std::vector<VertexTextured>&        vertices        = mMesh.GetVertices();
        std::vector<VertexTextured>&              frameVertices   = mMesh.GetFrameVertices();
        const std::vector<Mesh::VertBoneIndices>& vertBoneIdxs    = mMesh.GetBoneIndices();
        const std::vector<Mesh::VertBoneWeights>& vertBoneWeights = mMesh.GetBoneWeights();

        for ( size_t i=0; i<vertices.size(); ++i )
        {
            const VertexTextured& inVert = vertices[i];
            VertexTextured& outVert = frameVertices[i];
            const Mesh::VertBoneIndices& boneIdxs = vertBoneIdxs[i];
            const Mesh::VertBoneWeights& boneWeights = vertBoneWeights[i];

            glm::vec4 skinnedPos =
                mPalette[mshIdx].mEntry[boneIdxs.idx0] *
                glm::vec4( inVert.x, inVert.y, inVert.z, 1.0f ) *
                boneWeights.weight0;
            skinnedPos +=
                mPalette[mshIdx].mEntry[boneIdxs.idx1] *
                glm::vec4( inVert.x, inVert.y, inVert.z, 1.0f ) *
                boneWeights.weight1;
            skinnedPos +=
                mPalette[mshIdx].mEntry[boneIdxs.idx2] *
                glm::vec4( inVert.x, inVert.y, inVert.z, 1.0f ) *
                boneWeights.weight2;
            skinnedPos +=
                mPalette[mshIdx].mEntry[boneIdxs.idx3] *
                glm::vec4( inVert.x, inVert.y, inVert.z, 1.0f ) *
                boneWeights.weight3;

            outVert.x = skinnedPos.x;
            outVert.y = skinnedPos.y;
            outVert.z = skinnedPos.z;

            //printf("Skinned xyz: %f,%f,%f\n", outVert.x*0.001f,outVert.y*0.001f,outVert.z*0.001f);

            // TODO - transform normals?
        }
        mMesh.GetVertexBuffer().UpdateVertices(
            frameVertices.data(),
            frameVertices.size()
        );
    }
}

void AssimpMesh::SetAnim(const std::string& name, bool loop)
{
    // TODO - handle no looping
    (void)loop;
    for ( size_t i=0; i<mAnimNames.size(); ++i ) {
        if ( name == mAnimNames[i] ) {
            mAnimation = &mAnimations[i];
            mAnimTime = 0.0f;
            break;
        }
    }
}

void AssimpMesh::SetAnimTime( const float time ) {
    if ( !mAnimation ) { return; }
    if ( time >= 0.0f && time < mAnimation->GetDuration() ) {
        mAnimTime = time;
    }
}

void AssimpMesh::SetTexture( const Texture* tex, size_t meshIdx )
{
    if ( meshIdx >= mTextures.size() ) {
        mTextures.resize(meshIdx+1);
    }
    mTextures[meshIdx] = tex;
}

void AssimpMesh::SetPosition( const glm::vec3& pos ) { mTransform.position = pos; }
void AssimpMesh::SetRotation( const glm::quat& rot ) { mTransform.rotation = rot; }
void AssimpMesh::SetScale( const glm::vec3& scl ) { mTransform.scale = scl; }

void AssimpMesh::Draw(void)
{
    glm::mat4 modelMat = mTransform.ToMat4();

    Renderer* rndr = Renderer::GetInstance();
    for (size_t i=0; i<mMeshes.size(); ++i) {
        if (mTextures.size() > i && mTextures[i]) {
            rndr->SetTexture(*((Texture*)mTextures[i]));
        }
        rndr->DrawVertexBuffer( modelMat, mMeshes[i].GetVertexBuffer() );
    }
}

float AssimpMesh::GetCurAnimLength() const {
    if ( mAnimation ) {
        return mAnimation->GetDuration();
    }
    return 0.0f;
}

void AssimpMesh::ComputeMatrixPalette( const size_t mshIdx )
{
    Skeleton* mSkeleton = &mSkeletons[mshIdx];
    const std::vector<glm::mat4>& globalInvBindPoses =
        mSkeleton->GetGlobalInvBindPoses();
    mAnimation->GetGlobalPoseAtTime( mCurrentPoses[mshIdx], mSkeleton, mAnimTime );

    // setup the palette for each bone
    for ( size_t i=0; i<mSkeleton->GetNumBones(); ++i ) {
        // Global inverse bind pose matrix times current pose matrix
        mPalette[mshIdx].mEntry[i] = mCurrentPoses[mshIdx][i] * globalInvBindPoses[i];
    }
}

// Mesh functions
AssimpMesh::Mesh::Mesh()
{}
AssimpMesh::Mesh::~Mesh()
{}

bool AssimpMesh::Mesh::Load( const aiMesh* mesh, const aiScene* scene )
{
    const aiMesh* assimpMesh = mesh;

    mVertices.resize( assimpMesh->mNumVertices );
    mFrameVertices.resize( assimpMesh->mNumVertices );
    mBoneIndices.resize( assimpMesh->mNumVertices );
    mBoneWeights.resize( assimpMesh->mNumVertices );
    std::vector<unsigned int> boneWeightCounters( assimpMesh->mNumVertices, 0 );

    for (size_t i=0; i<assimpMesh->mNumVertices; ++i) {
        mVertices[i].x = assimpMesh->mVertices[i].x;
        mVertices[i].y = assimpMesh->mVertices[i].y;
        mVertices[i].z = assimpMesh->mVertices[i].z;
        mVertices[i].nx = assimpMesh->mNormals[i].x;
        mVertices[i].ny = assimpMesh->mNormals[i].y;
        mVertices[i].nz = assimpMesh->mNormals[i].z;
        if ( assimpMesh->mTextureCoords[0] ) {
            mVertices[i].u = assimpMesh->mTextureCoords[0][i].x;
            mVertices[i].v = assimpMesh->mTextureCoords[0][i].y;
        } else {
            mVertices[i].u = mVertices[i].v = 0.0f;
        }

        // default all bone indices to 0
        mBoneIndices[i].idx0 = 0;
        mBoneIndices[i].idx1 = 0;
        mBoneIndices[i].idx2 = 0;
        mBoneIndices[i].idx3 = 0;

        // default all weights to 0
        mBoneWeights[i].weight0 = 0.0f;
        mBoneWeights[i].weight1 = 0.0f;
        mBoneWeights[i].weight2 = 0.0f;
        mBoneWeights[i].weight3 = 0.0f;
    }

    for ( size_t i=0; i<assimpMesh->mNumBones; ++i ) {
        const aiBone* bone = assimpMesh->mBones[i];
        //std::string boneName(bone->mName.C_Str());

        // get weights/indices
        for ( size_t j=0; j<bone->mNumWeights; ++j ) {
            const aiVertexWeight& weight = bone->mWeights[j];
            // index of the vertex which is influenced by the bone
            const unsigned int vertIndex = weight.mVertexId;
            assert( vertIndex < mVertices.size() );
            switch ( boneWeightCounters[vertIndex] )
            {
            case 0:
                mBoneWeights[vertIndex].weight0 = weight.mWeight;
                mBoneIndices[vertIndex].idx0 = uint32_t(i);
                break;
            case 1:
                mBoneWeights[vertIndex].weight1 = weight.mWeight;
                mBoneIndices[vertIndex].idx1 = uint32_t(i);
                break;
            case 2:
                mBoneWeights[vertIndex].weight2 = weight.mWeight;
                mBoneIndices[vertIndex].idx2 = uint32_t(i);
                break;
            case 3:
                mBoneWeights[vertIndex].weight3 = weight.mWeight;
                mBoneIndices[vertIndex].idx3 = uint32_t(i);
                break;
            default:
                std::cout << "AssimpMesh unhandled weight number " <<
                    boneWeightCounters[vertIndex] << std::endl;
                break;
            }
            ++boneWeightCounters[vertIndex];
        }
    }
    // possibly todo - normalize weights?

    // copy of original vertices to be updated each frame
    mFrameVertices = mVertices;

    std::vector<uint32_t> indices( assimpMesh->mNumFaces*3 );
    for ( size_t i=0; i<assimpMesh->mNumFaces; ++i ) {
        aiFace& face = assimpMesh->mFaces[i];
        assert( face.mNumIndices == 3 );
        indices[i*3 + 0] = face.mIndices[0];
        indices[i*3 + 1] = face.mIndices[1];
        indices[i*3 + 2] = face.mIndices[2];
    }

    mVertBuf = std::make_shared<VertexBuffer>(
        VertexBuffer::POS_TEXCOORD,
        mVertices.data(), mVertices.size(),
        indices.data(), indices.size(),
        VertexBuffer::USAGE_DYNAMIC
    );

    return true;
}

static const aiNode* getBoneNode(
    const std::string& name,
    const aiNode* node
)
{
    if ( node != nullptr &&
            std::string(node->mName.C_Str()) == name) {
        return node;
    }
    
    for ( size_t i=0; i<node->mNumChildren; ++i ) {
        const aiNode* childNode = getBoneNode( name, node->mChildren[i] );
        if ( childNode != nullptr ) {
            return childNode;
        }
    }
    
    return nullptr;
}
static std::string getParentName(
    const std::string& boneName,
    const aiScene* scene
)
{
    const aiNode* rootNode = scene->mRootNode;
    const aiNode* boneNode = getBoneNode( boneName, rootNode );
    assert( boneNode != nullptr );
    if ( boneNode->mParent != nullptr ) {
        const aiNode* parent = boneNode->mParent;
        return std::string( parent->mName.C_Str() );
    }
    return "";
}

// Skeleton functions
bool AssimpMesh::Skeleton::Load(
    const aiMesh* mesh,
    const aiScene* scene
)
{
    mBones.resize( mesh->mNumBones );
    for ( size_t i=0; i<mesh->mNumBones; ++i ) {
        const aiBone* bone = mesh->mBones[i];
        mBones[i].mLocalBindPose = aiMatToMat4( bone->mOffsetMatrix );
        mBones[i].mName = std::string( bone->mName.C_Str() );
        mBones[i].mParent = -1;
    }
    // Get parent indices
    for ( size_t i=0; i<mBones.size(); ++i ) {
        std::string parentName = getParentName( mBones[i].mName, scene );
        if ( !parentName.empty() ) {
            for ( size_t j=0; j<mBones.size(); ++j ) {
                if ( mBones[j].mName == parentName ) {
                    mBones[i].mParent = j;
                }
            }
        }
    }
    // Get the root index
    mRootBoneIdx = 0;
    for ( size_t i=0; i<mBones.size(); ++i ) {
        if ( mBones[i].mParent == -1 ) {
            mRootBoneIdx = i;
            break;
        }
    }
    ComputeGlobalInvBindPose();
    return true;
}

void AssimpMesh::Skeleton::ComputeGlobalInvBindPose()
{
    // resize to number of bones, which auto fills identity
    mGlobalInvBindPoses.resize( GetNumBones() );
    if ( GetNumBones() == 0 ) { return; }
    
    // Assimp already calculates the global inverse bind poses for us
    mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose;
    for ( size_t i=1; i<mGlobalInvBindPoses.size(); ++i ) {
        const glm::mat4 localMat = mBones[i].mLocalBindPose;
        mGlobalInvBindPoses[i] = localMat;
    }
}

// Animation functions
bool AssimpMesh::Animation::Load(
    const aiAnimation* assimpAnim,
    const aiScene* scene,
    const Skeleton* skeleton
)
{
    const std::vector<Skeleton::Bone>& bones = skeleton->GetBones();
    mName = std::string( assimpAnim->mName.C_Str() );
    mTracks.resize( bones.size() );
    mNumBones = mTracks.size();
    mNumFrames = assimpAnim->mChannels[0]->mNumPositionKeys;
    // possibly TODO - properly convert these?
    //mDuration = assimpAnim->mDuration / assimpAnim->mTicksPerSecond;
    //mFrameDuration = mDuration / float(mNumFrames);
    mFrameDuration = 1.0f/24.0f; // default 24 fps
    mDuration = (mNumFrames-1) * mFrameDuration;
    for ( size_t i=0; i<mTracks.size(); ++i ) {
        mTracks[i].resize(mNumFrames);
    }
    for ( size_t i=0; i<assimpAnim->mNumChannels; ++i ) {
        const aiNodeAnim* channel = assimpAnim->mChannels[i];
        std::string name = std::string( channel->mNodeName.C_Str() );
        size_t boneIdx = 0;
        for ( size_t j=0; j<bones.size(); ++j ) {
            if ( bones[j].mName == name ) {
                boneIdx = j;
            }
        }
        
        assert( mNumFrames == channel->mNumPositionKeys );
        assert(
            (channel->mNumPositionKeys == channel->mNumRotationKeys) &&
            (channel->mNumPositionKeys == channel->mNumScalingKeys)
        );
        //mTracks[boneIdx].resize(channel->mNumPositionKeys);
        for ( size_t j=0; j<channel->mNumPositionKeys; ++j ) {
            const aiVectorKey& keyFrame = channel->mPositionKeys[j];
            mTracks[boneIdx][j].position = glm::vec3(
                keyFrame.mValue.x,
                keyFrame.mValue.y,
                keyFrame.mValue.z
            );
        }
        for ( size_t j=0; j<channel->mNumRotationKeys; ++j ) {
            const aiQuatKey& keyFrame = channel->mRotationKeys[j];
            mTracks[boneIdx][j].rotation = glm::quat(
                keyFrame.mValue.w,
                keyFrame.mValue.x,
                keyFrame.mValue.y,
                keyFrame.mValue.z
            );
        }
        for ( size_t j=0; j<channel->mNumScalingKeys; ++j ) {
            const aiVectorKey& keyFrame = channel->mScalingKeys[j];
            mTracks[boneIdx][j].scale = glm::vec3(
                keyFrame.mValue.x,
                keyFrame.mValue.y,
                keyFrame.mValue.z
            );
        }
    }
    return true;
}

void AssimpMesh::Animation::GetGlobalPoseAtTime(
    std::vector<glm::mat4>& outPoses,
    const Skeleton* inSkeleton,
    float inTime ) const
{
    if ( outPoses.size() != mNumBones ) {
        outPoses.resize( mNumBones );
    }

    // Figure out the current frame idx and next.
    // Assumes inTime is in range [0,mDuration]
    size_t frame = static_cast<size_t>( inTime/mFrameDuration );
    size_t nextFrame = frame+1;
    // Calculate percentage between this and next frame
    float pct = inTime / mFrameDuration - frame;

    // setup root pose
    size_t rootIdx = inSkeleton->GetRootBoneIdx();
    if ( mTracks.size() > 0 ) {
        Transform interp = Lerp(
            mTracks[rootIdx][frame],
            mTracks[rootIdx][nextFrame],
            pct
        );
        outPoses[rootIdx] = interp.ToMat4();
    } else {
        outPoses[rootIdx] = glm::mat4(1.0f); // identity
    }

    // setup remaining poses
    const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
    for ( size_t bone=0; bone<mNumBones; ++bone ) {
        glm::mat4 localMat(1.0f);
        if ( mTracks[bone].size() > 0 ) {
            Transform interp = Lerp(
                mTracks[bone][frame],
                mTracks[bone][nextFrame],
                pct
            );
            localMat = interp.ToMat4();
        }
        if ( bones[bone].mParent >= 0 ) {
            outPoses[bone] = outPoses[bones[bone].mParent] * localMat;
        }
    }
}


