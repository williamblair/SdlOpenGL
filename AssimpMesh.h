#ifndef ASSIMP_MESH_H_INCLUDED
#define ASSIMP_MESH_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexBuffer.h"
#include "Texture.h"
#include "Transform.h"

class AssimpMesh
{
public:

    AssimpMesh( const std::string& fileName );
    ~AssimpMesh();

    void Update     ( const float dt );
    void SetAnim    ( const std::string& name, bool loop = true );
    void SetAnimTime( const float time );
    void SetTexture ( const Texture* tex, size_t meshIdx = 0 );
    void SetPosition( const glm::vec3& pos );
    void SetRotation( const glm::quat& rot );
    void SetScale   ( const glm::vec3& scl );

    void Draw(void);

    const std::vector<std::string>& GetAnimNames(void) const { return mAnimNames; }
    float GetCurAnimLength(void) const;
    float GetCurAnimTime  (void) const { return mAnimTime; }

private:

    class Mesh
    {
    public:

        struct VertBoneIndices
        {
            uint32_t idx0;
            uint32_t idx1;
            uint32_t idx2;
            uint32_t idx3;
        };
        struct VertBoneWeights
        {
            float weight0;
            float weight1;
            float weight2;
            float weight3;
        };

        Mesh();
        ~Mesh();

        bool Load( const aiMesh* mesh, const aiScene* scene );
        void Unload(void);

        VertexBuffer&                       GetVertexBuffer() { return *mVertBuf; }
        const std::string&                  GetFileName() const { return mFileName; }
        const std::vector<VertexTextured>&  GetVertices() const { return mVertices; }
        std::vector<VertexTextured>&        GetFrameVertices() { return mFrameVertices; }
        const std::vector<VertBoneIndices>& GetBoneIndices() const { return mBoneIndices; }
        const std::vector<VertBoneWeights>& GetBoneWeights() const { return mBoneWeights; }

    private:

        std::shared_ptr<VertexBuffer> mVertBuf;
        std::vector<VertexTextured> mVertices;
        std::vector<VertexTextured> mFrameVertices; // copy of mVertices, to be modified each frame for animation
        std::vector<VertBoneIndices> mBoneIndices;
        std::vector<VertBoneWeights> mBoneWeights;
        std::string mFileName;
    };

    static const size_t MAX_SKELETON_BONES = 96;
    struct MatrixPalette
    {
        glm::mat4 mEntry[MAX_SKELETON_BONES];
    };

    class Skeleton
    {
    public:

        struct Bone
        {
            glm::mat4 mLocalBindPose;
            std::string mName;
            int mParent;
        };

        bool Load( const aiMesh* assimpMesh, const aiScene* scene );

        size_t                        GetNumBones(void)           const { return mBones.size(); }
        size_t                        GetRootBoneIdx(void)        const { return mRootBoneIdx; }
        const Bone&                   GetBone( size_t idx )       const { return mBones[idx]; }
        const std::vector<Bone>&      GetBones(void)              const { return mBones; }
        const std::vector<glm::mat4>& GetGlobalInvBindPoses(void) const { return mGlobalInvBindPoses; }
        const std::string&            GetFileName(void)           const { return mFileName; }

    protected:
        // Called automatically when the skeleton is loaded
        // Computes the global inverse bind pose for each bone
        void ComputeGlobalInvBindPose();
    private:
        // The bones in the skeleton
        std::vector<Bone> mBones;
        // The global inverse bind poses for each bone
        std::vector<glm::mat4> mGlobalInvBindPoses;
        // The file this was loaded from
        std::string mFileName;
        // which index into mBones is the root node
        size_t mRootBoneIdx;
    };

    class Animation
    {
    public:

        bool Load( const aiAnimation* assimpAnim, const aiScene* scene, const Skeleton* skeleton );

        size_t GetNumBones() const { return mNumBones; }
        size_t GetNumFrames() const { return mNumFrames; }
        float GetDuration() const { return mDuration; }
        float GetFrameDuration() const { return mFrameDuration; }
        const std::string& GetName() const { return mName; }
        void SetName( const std::string& newName ) { mName = newName; }

        // Fills the provided vector with the global (current) pose matrices
        // for each bone at the specified time in the anim.
        // Time must be >= 0.0f && <= mDuration.
        void GetGlobalPoseAtTime(
            std::vector<glm::mat4>& outPoses,
            const Skeleton* inSkeleton,
            float inTime
        ) const;

    private:
        size_t mNumBones;
        size_t mNumFrames;

        float mDuration; // total anim length in seconds
        float mFrameDuration; // length of 1 frame in seconds

        // transform info for each frame on the track.
        // each index in the outer vector is a bone,
        // inner vector is a frame
        std::vector<std::vector<Transform>> mTracks;

        // file this was loaded from
        std::string mName;
    };

    std::vector<MatrixPalette> mPalette;
    std::vector<Mesh> mMeshes;
    std::vector<const Texture*> mTextures;
    std::vector<Skeleton> mSkeletons;
    std::vector<Animation> mAnimations;
    std::vector<std::string> mAnimNames;
    Animation* mAnimation;
    float mAnimPlayRate;
    float mAnimTime;
    std::vector<std::vector<glm::mat4>> mCurrentPoses;
    Transform mTransform;

    bool processNode(
        const aiNode* node,
        const aiScene* scene,
        size_t& curMesh
    );
    bool processAnimation(
        const aiAnimation* assimpAnim,
        const aiScene* scene,
        Animation& anim
    );
    void ComputeMatrixPalette( const size_t mshIdx );
};

#endif

