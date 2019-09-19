#ifndef MATH3D_H
#define MATH3D_H
#include "glm/glm.hpp"

namespace Math3D
{
    float Disorientation(   const glm::vec3 & face_direction, 
                            const glm::vec3 & object_direction,
                            const glm::vec3 & positive_disorientation);

}
#endif