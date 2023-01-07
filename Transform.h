#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    inline Transform() :
        position(0.0f,0.0f,0.0f),
        rotation(1.0f,0.0f,0.0f,0.0f), // wxyz
        scale(1.0f,1.0f,1.0f)
    {}

    inline glm::mat4 ToMat4() {
        return
            glm::translate( glm::mat4(1.0f), position ) *
            glm::mat4_cast( rotation ) *
            glm::scale( glm::mat4(1.0f), scale );
    }
};

inline Transform Lerp( const Transform& a, const Transform& b, const float pct ) {
    Transform result;
    result.position = glm::mix( a.position, b.position, pct );
    result.rotation = glm::slerp( a.rotation, b.rotation, pct );
    result.scale = glm::mix( a.scale, b.scale, pct );
    return result;
}

#endif // TRANSFORM_H_INCLUDED

