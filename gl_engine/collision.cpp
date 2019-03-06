#include "collision.h"
namespace Collision
{
    std::pair<float, float> ProjectEdgesOnAxe( const glm::mat4 &model_matrix,const std::vector<std::pair<glm::vec3, glm::vec3>> &edges,const glm::vec3 &position,const glm::vec3 & axe)
    {
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        glm::vec3 norm;
        glm::vec4 norm4;
        float val;
        for(auto edge :edges)
        {

            norm4 = model_matrix * glm::vec4(edge.second[0],edge.second[1],edge.second[2],0.0f);
            norm = position + glm::vec3(norm4[0],norm4[1],norm4[2]) ;

            val = glm::dot(norm,axe);

            if(val < min) min = val;

            if(val > max) max = val;

            

        }
        return std::pair<float, float>(min, max);
        
    }

    int AddAxes(std::vector<glm::vec3> &axes,
                const std::vector<std::pair<glm::vec3, glm::vec3>> &m_edges,
                const glm::mat4 &model_matrix)
    {
        glm::vec4 norm4;
        glm::vec3 norm;
        glm::vec3 norm_z(0.0f,0.0f,1.0f);

        for(auto edge :m_edges)
        {
            norm4 = model_matrix * glm::vec4(edge.second[0],edge.second[1],edge.second[2],0.0f);
            norm = glm::vec3(norm4[0],norm4[1],norm4[2]);        
            axes.push_back(glm::normalize(norm));
            norm = edge.second - edge.first;
            norm = glm::cross(norm_z,norm);
            norm4 = model_matrix * glm::vec4(norm[0],norm[1],norm[2],0.0f);
            norm = glm::vec3(norm4[0],norm4[1],norm4[2]);
            axes.push_back(glm::normalize(norm));
        }
        
        return 2 * m_edges.size();
    }
}