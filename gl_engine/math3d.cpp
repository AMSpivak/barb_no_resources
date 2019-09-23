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

        bool IsCounterClockwiseTriangle(    const glm::vec3 & a1, 
                                    const glm::vec3 & a2,
                                    const glm::vec3 & a3,
                                    const glm::vec3 & normal,
                                    )
        {
            const glm::vec3 a12(a2- a1);
            const glm::vec3 a13(a3- a1);
            return  glm::dot(normal,(glm::cross(a13,a12))) > 0.0f;
        }
    
}