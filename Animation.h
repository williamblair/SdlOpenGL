#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <string>
#include <vector>

#include "KeyFrame.h"

struct Animation
{
    std::string name;
    std::vector<KeyFrameRot> rotKeyFrames;
    std::vector<KeyFrameTrans> transKeyFrames;
    std::vector<KeyFrameScale> scaleKeyFrames;
};

#endif // ANIMATION_H_INCLUDED
