#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <cstdio>
#include <string>
#include <vector>

#include "KeyFrame.h"

struct NodeAnimation
{
    int node; // index into mNodes
    std::vector<KeyFrameRot> rotKeyFrames;
    std::vector<KeyFrameTrans> transKeyFrames;
    std::vector<KeyFrameScale> scaleKeyFrames;
    
    void print()
    {
        printf( "node: %d\n", node );
        printf( "Rot keyframes size: %lu\n", rotKeyFrames.size() );
        if ( rotKeyFrames.size() > 0 )
        {
            for ( KeyFrameRot& rot : rotKeyFrames )
            {
                float* fPtr = glm::value_ptr( rot.rotation );
                printf( "Time: %f, Rot: %f,%f,%f,%f\n",
                    rot.time,
                    fPtr[0],
                    fPtr[1],
                    fPtr[2],
                    fPtr[3] );
            }
        }
        printf( "Trans keyframes size: %lu\n", transKeyFrames.size() );
        if ( transKeyFrames.size() > 0 )
        {
            for ( KeyFrameTrans& trans : transKeyFrames )
            {
                float* fPtr = glm::value_ptr( trans.translation );
                printf( "Time: %f, Trans: %f,%f,%f\n",
                    trans.time,
                    fPtr[0],
                    fPtr[1],
                    fPtr[2] );
            }
        }
        printf( "Scale keyframes size: %lu\n", scaleKeyFrames.size() );
        if ( scaleKeyFrames.size() > 0 )
        {
            for ( KeyFrameScale& scale : scaleKeyFrames )
            {
                float* fPtr = glm::value_ptr( scale.scale );
                printf( "Time: %f, Scale: %f,%f,%f\n",
                    scale.time,
                    fPtr[0],
                    fPtr[1],
                    fPtr[2] );
            }
        }
    }
};

struct Animation
{
    std::string name;
    std::vector<NodeAnimation> nodeAnims;
    
    void print()
    {
        printf( "Anim name: %s\n", name.c_str() );
        printf( "Node anims:\n");
        for ( NodeAnimation& anim : nodeAnims )
        {
            anim.print();
        }
    }
};

#endif // ANIMATION_H_INCLUDED
