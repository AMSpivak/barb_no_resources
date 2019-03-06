#ifndef GL_MAP_HEIGHTMAP_H
#define GL_MAP_HEIGHTMAP_H
#include "gl_resources_manager.h"
#include <memory>

#include "glm/glm.hpp"

// #include "glm/trigonometric.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"
// #include "glm/gtx/rotate_vector.hpp"
namespace GameMap
{
    class HeightMap
    {
        public:
        HeightMap():m_heightmap(nullptr),m_map_size(10.0f,10.0f,10.0f){}

        ~HeightMap();
        void LoadMap(std::string FileName);
        void SetParameters(float map_scaler,float height_scaler);
        float GetHeight(float x,float y);
        float GetMapScaler();
        float GetHeightScaler();
        void Draw(GLuint current_shader,const glm::vec3 &position,const glm::mat4 camera);
        const glm::vec3 GetMapSize();
        void SetMapSize(const glm::vec3 &size);
        std::shared_ptr<IGlTextureStruct> m_heightmap_texture;
        std::shared_ptr<IGlTextureStruct> m_texture;

        private:
        float m_map_scaler;
        float m_height_scaler;
        glm::vec3  m_map_size;       
        int m_height;
        int m_width;

        unsigned char * m_heightmap;
    };
    
}

#endif