#include "math3d.h"
namespace Math3D
{
    float Disorientation(   const glm::vec3 & face_direction, 
                            const glm::vec3 & object_direction, 
                            const glm::vec3 & positive_disorientation)
    {
        float disorientation = 1.0f - glm::dot(face_direction,object_direction);
        if(glm::dot(positive_disorientation,object_direction)< 0.0f)
        {
            disorientation = -disorientation;
        }
        return disorientation;
    }
    
}