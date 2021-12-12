#ifndef KEYFRAME_H_INCLUDED
#define KEYFRAME_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

struct KeyFrameRot
{
    float time;
    glm::quat rotation;
};

struct KeyFrameTrans
{
    float time;
    glm::vec3 translation;
};

struct KeyFrameScale
{
    float time;
    glm::vec3 scale;
};

#endif // KEYFRAME_H_INCLUDED
