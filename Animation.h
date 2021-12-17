#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

#include "KeyFrame.h"

struct NodeAnimation
{
    int node; // index into mNodes
    std::vector<KeyFrameRot> rotKeyFrames;
    std::vector<KeyFrameTrans> transKeyFrames;
    std::vector<KeyFrameScale> scaleKeyFrames;
    float startTime;
    float endTime;
    
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
    float startTime;
    float endTime;
    
    void print()
    {
        printf( "Anim name: %s\n", name.c_str() );
        printf( "Node anims:\n");
        for ( NodeAnimation& anim : nodeAnims )
        {
            anim.print();
        }
    }
    
    void calcStartEndTime()
    {
        /* Find the min and max keyframe times for start and end time */
        float minTime = 10000.0f;
        float maxTime = 0.0f;
        for ( NodeAnimation& nodeAnim : nodeAnims )
        {
            for ( size_t i = 0; i < nodeAnim.rotKeyFrames.size() - 1; ++i )
            {
                assert( nodeAnim.rotKeyFrames[i].time < nodeAnim.rotKeyFrames[i+1].time );
            }
            for ( size_t i = 0; i < nodeAnim.transKeyFrames.size() - 1; ++i )
            {
                assert( nodeAnim.transKeyFrames[i].time < nodeAnim.transKeyFrames[i+1].time );
            }
            for ( size_t i = 0; i < nodeAnim.scaleKeyFrames.size() - 1; ++i )
            {
                assert( nodeAnim.scaleKeyFrames[i].time < nodeAnim.scaleKeyFrames[i+1].time );
            }
            
            std::vector<KeyFrameRot>& rotFrames = nodeAnim.rotKeyFrames;
            std::vector<KeyFrameTrans>& transFrames = nodeAnim.transKeyFrames;
            std::vector<KeyFrameScale>& scaleFrames = nodeAnim.scaleKeyFrames;
            
            for ( size_t i = 0;
                  i < std::max( rotFrames.size(),
                                std::max( transFrames.size(),
                                          scaleFrames.size() ));
                  ++i )
            {
                if ( i < rotFrames.size() ) {
                    if ( rotFrames[i].time < minTime ) minTime = rotFrames[i].time;
                    if ( rotFrames[i].time > maxTime ) maxTime = rotFrames[i].time;
                }
                if ( i < transFrames.size() ) {
                    if ( transFrames[i].time < minTime ) minTime = transFrames[i].time;
                    if ( transFrames[i].time > maxTime ) maxTime = transFrames[i].time;
                }
                if ( i < scaleFrames.size() ) {
                    if ( scaleFrames[i].time < minTime ) minTime = scaleFrames[i].time;
                    if ( scaleFrames[i].time > maxTime ) maxTime = scaleFrames[i].time;
                }
            }
        }
        
        startTime = minTime;
        endTime = maxTime;
    }
};

#endif // ANIMATION_H_INCLUDED
