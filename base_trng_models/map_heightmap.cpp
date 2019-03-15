#include "map_heightmap.h"
#include "glresourses.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace GameMap
{

    HeightMap::~HeightMap()
    {
        if(m_heightmap != nullptr)
        {
            DeleteHeightMap(m_heightmap);
        }

    }
    void HeightMap::LoadMap(std::string FileName)
    {
        m_heightmap = LoadHeightMap(GetResourceManager()->m_texture_atlas.GetResourceFolder() + FileName,&m_width, &m_height);
        
        m_heightmap_texture = GetResourceManager()->m_texture_atlas.Assign(FileName);

        m_material = std::make_shared<GameResource::GlMaterial>("tile_diff.png","tile_norm.png","base_l.png");
    }

    void HeightMap::SetParameters(float map_scaler,float height_scaler)
    {
        m_map_scaler = map_scaler;
        m_height_scaler = height_scaler;
    }

    size_t inline MapOffset(size_t ix,size_t iy, size_t line_width, size_t lines_count, size_t pixel_size)
    {
        if(ix < 0) ix+= line_width;
        if(iy < 0) iy+= lines_count;

        ix %= line_width;
        iy %= lines_count;

        size_t offset = iy; 
        offset = (offset * line_width + ix);
        return offset * pixel_size;
    }
    
    float HeightMap::GetHeight(float x,float y)
    {
        if(m_heightmap == nullptr)
            return 0.0f;

        float fx =(x * m_map_scaler + 0.5f) * m_width;
        float fy =(y * m_map_scaler + 0.5f) * m_height;
         

        int ix = fx;
        int iy = fy;

        ix %= m_width;
        iy %= m_height;

        float off_x = fx - ix;
        float off_y = fy - iy;

        size_t  offset = MapOffset(ix,iy,m_width,m_height,4);    
        float h0 = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix+1,iy,m_width,m_height,4);    
        float hx = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix,iy+1,m_width,m_height,4);    
        float hy = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix+1,iy+1,m_width,m_height,4);    
        float hxy = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        float sx1 = h0 + off_x * (hx - h0);
        float sx2 = hy + off_x * (hxy - hy);

        return sx1 + off_y * (sx2 - sx1);
    }

    float HeightMap::GetMapScaler()
    {
        return m_map_scaler;
    }

    float HeightMap::GetHeightScaler()
    {
        return m_height_scaler;
    }

    const glm::vec3 HeightMap::GetMapSize()
    {
        return m_map_size;
    }
    
    void HeightMap::SetMapSize(const glm::vec3 &size)
    {
        m_map_size = size;
    }

    void HeightMap::Draw(GLuint current_shader,const glm::vec3 &position,const glm::mat4 camera)
    {
        glUseProgram(current_shader);
        const float tile_size = 1.0f;
        float inv = 1/tile_size;
        //glm::vec3 = glm::fract(position);
        float x = position[0]-trunc(position[0]);
        float z = position[2]-trunc(position[2]);

        float offset_x = x - (tile_size * round(x*inv)); 
        float offset_z = z - (tile_size * round(z*inv)); 

        glm::vec4 offset_position_vector = glm::vec4(-offset_x,-position[1],-offset_z,tile_size);
        glm::vec4 map_position_vector = glm::vec4(position[0],position[2],GetMapScaler(),GetHeightScaler());
        
        GLuint cameraLoc  = glGetUniformLocation(current_shader, "camera");
        glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(camera));

        GLuint offset_position  = glGetUniformLocation(current_shader, "offset_position");
        glUniform4fv(offset_position, 1, glm::value_ptr(offset_position_vector));

        GLuint map_position  = glGetUniformLocation(current_shader, "map_position");
        glUniform4fv(map_position, 1, glm::value_ptr(map_position_vector));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_heightmap_texture->m_texture);

        glUniform1i(glGetUniformLocation(current_shader, "AlbedoTexture"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_material->m_albedo_texture->m_texture);

        glUniform1i(glGetUniformLocation(current_shader, "NormalTexture"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_material->m_normal_height_texture->m_texture);
        RenderHeightMap();
    }
    
}